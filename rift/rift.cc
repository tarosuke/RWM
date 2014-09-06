#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>
#include <assert.h>
#include <linux/types.h>
#include <linux/input.h>
#include <linux/hidraw.h>

#include "rift.h"
#include "../settings/settings.h"


#define MaxFloat (3.40282347e+38F)
#define G (9.80665)


/////ヘッドトラッカー関連

void RIFT::UpdateAngularVelocity(const int angles[3], double dt){
	COMPLEX<4> delta(angles, 0.0001 * dt);
	pose.direction *= delta;
}

void RIFT::UpdateAccelaretion(const int axis[3], double dt){
	//位置や速度を求める
	if(dt <= 0.02){ //dtが異常に大きい時はデータが欠損しているので位置更新しない
		//加速度情報取得
		VECTOR<3> acc(axis, 0.0001);
		const double scale(acc.Length());
		acc.Rotate(pose.direction); //絶対座標系へ変換
		acc *= scale;

		//重力加速度分離
		VECTOR<3> g(acc);
		g *= correctionGain;
		gravity *= 1.0 - correctionGain;
		gravity += g;

		if(InitialProgress() < 0.9){
			//初期キャリブレーション中なので停止
			velocity *= 0;
			pose.position *= 0;
			gravity[1] = G;
		}else{
			//重力を除去
			acc -= gravity;

			//速度、位置を算出
			acc *= dt;
			velocity += acc;
			VECTOR<3> v(velocity);
			v *= dt;
			pose.position += v;

			//枠を出ていたら止める
			const float limit(1.0);
			double* p(pose.position);
			double* ve(velocity);
			for(unsigned n(0); n < 3; ++n){
				if(p[n] < -limit){
					ve[n] = 0;
					p[n] = -limit;
				}else if(limit < p[n]){
					ve[n] = 0;
					p[n] = limit;
				}
			}

			double distance(pose.position.Length() * 0.5);
			double lead(1.0 - distance*distance*distance*distance);

			velocity *= lead;
			pose.position *= lead;
		}
	}
}

void RIFT::UpdateMagneticField(const int axis[3]){
	//磁力情報取得
	VECTOR<3> mag(axis);

	//キャリブレーション
	if(!magReady){
		//磁化情報をある程度得たらそれ以上は更新しない。
		magMax.Max(mag);
		magMin.Min(mag);
	}
	const VECTOR<3> deGain(magMax - magMin);
	const double* const d(deGain);

	if(magReady){
		//磁化分を除去
		VECTOR<3> offset(magMax + magMin);
		offset *= 0.5;
		mag -= offset;

		//絶対座標系に変換
		mag.Rotate(pose.direction);
		double* v(mag);
		v[1] = 0; //邪魔なので鉛直を消去
		mag.Normalize();

		//平均化処理
		mag *= correctionGain;
		magneticField += mag;;
		magneticField.Normalize();;
	}else{
		//キャリブレーション可能判定
		if(6000 < abs(d[0]) && 6000 < abs(d[1]) && 6000 < abs(d[2])){
			magReady = true;
			averageRatio = 3;
			puts("magnetic azimuth correction READY.");
		}
	}
}


const VECTOR<3> RIFT::vertical((const double[3]){0,-1,0});
void RIFT::ErrorCorrection(){
	COMPLEX<4> diff;

	//重力補正
	COMPLEX<4> gdiff(gravity, vertical);
	gdiff.Normalize();
	gdiff *= correctionGain;
	const double gl(gravity.Length());
	gravity.Rotate(gdiff);
	gravity *= gl;
	diff *= gdiff;

	//磁気補正
	COMPLEX<4> mdiff(magneticField, vNorth);
	mdiff.FilterAxis(2);
	mdiff.Normalize();
	mdiff *= correctionGain;
	magneticField.Rotate(mdiff);
	diff *= mdiff;

	diff *= pose.direction;
	pose.direction = diff;

	//平均化、補正レートの更新
	if(averageRatio < maxAverageRatio){
		correctionGain = 0.7 / averageRatio;
		++averageRatio;
	}
}


void* RIFT::KeepaliveThread(void* instance){
	//オブジェクトを設定して監視開始
	(*(RIFT*)instance).Keepalive();
	return 0;
}

void RIFT::Keepalive(){
	for(;;){
		static const char keepaliveCommand[5] ={
			8, 0, 0,
			(char)(keepaliveInterval & 0xff),
			(char)(keepaliveInterval >> 8)
		};
		ioctl(fd, HIDIOCSFEATURE(5), keepaliveCommand);
		usleep(keepaliveInterval * 500);
	}
}

void RIFT::DecodeSensor(const unsigned char* buff, int v[3]){
	struct {int x:21;} s;

	v[0] = s.x =
		((unsigned)buff[0] << 13) |
		((unsigned)buff[1] << 5) |
		((buff[2] & 0xfb) >> 3);
	v[1] = s.x =
		(((unsigned)buff[2] & 0x07) << 18) |
		((unsigned)buff[3] << 10) |
		((unsigned)buff[4] << 2) |
		((buff[5] & 0xc0) >> 6);
	v[2] = s.x =
		(((unsigned)buff[5] & 0x3f) << 15) |
		((unsigned)buff[6] << 7) |
		(buff[7] >> 1);
}

void RIFT::Decode(const char* buff){
	struct{
		int accel[3];
		int rotate[3];
	}sample[3];
	int mag[3];

	//NOTE:リトルエンディアン機で動かす前提
	const unsigned char numOfSamples(buff[1]);
	const unsigned short timestamp(*(unsigned short*)&buff[2]);
	//	const short temp(*(short*)&buff[6]);

	const uint samples(numOfSamples > 2 ? 3 : numOfSamples);
	for(unsigned char i(0); i < samples; i++){
		DecodeSensor((unsigned char*)buff + 8 + 16 * i, sample[i].accel);
		DecodeSensor((unsigned char*)buff + 16 + 16 * i, sample[i].rotate);
	}
	//磁気センサのデータ取得
	mag[0] = *(short*)&buff[56];
	mag[1] = *(short*)&buff[58];
	mag[2] = *(short*)&buff[60];

	static unsigned short prevTime;
	const unsigned short deltaT(timestamp - prevTime);
	prevTime = timestamp;

	const double qtime(1.0/1000.0);
	const double dt(qtime * deltaT / numOfSamples);

	// 各サンプル値で状況を更新
	for(unsigned char i(0); i < samples; i++){
		UpdateAngularVelocity(sample[i].rotate, dt);
		UpdateAccelaretion(sample[i].accel, dt);
	}

	// 磁界値取得
	UpdateMagneticField(mag);

	//温度取得
	UpdateTemperature(0.01 * *(short*)&buff[6]);

}


void RIFT::SensorThread(){
	//優先度設定
	pthread_setschedprio(
		sensorThread,
		sched_get_priority_max(SCHED_FIFO));

	//Riftからのデータ待ち、処理
	fd_set readset;

	FD_ZERO(&readset);
	FD_SET(fd, &readset);

	for(;; pthread_testcancel()){
		int result(select(fd + 1, &readset, NULL, NULL, NULL));

		if(result && FD_ISSET( fd, &readset )){
			char buff[256];
			const int rb(read(fd, buff, 256));
			if(62 == rb){
				Decode(buff);
				ErrorCorrection();
			}else{
				printf("%5d bytes dropped.\n", rb);
			}
		}
	}
}
void* RIFT::_SensorThread(void* initialData){
	//オブジェクトを設定して監視開始
	(*(RIFT*)initialData).SensorThread();
	return 0;
}

int RIFT::OpenDeviceFile(const int pid){
	//Riftのセンサを準備
	for(int i(0); i < 99; i++){
		char name[32];
		snprintf(name, 32, "/dev/hidraw%d", i);
		const int fd(open(name, O_RDWR | O_NONBLOCK));
		if(fd < 0){
			//開けなかった
			continue;
		}

		struct hidraw_devinfo info;
		if(ioctl(fd, HIDIOCGRAWINFO, &info) < 0){
			//ioctlできない=riftではない
			close(fd);
			continue;
		}
		if(VendorID != info.vendor || pid != info.product){
			//riftではない
			close(fd);
			continue;
		}

		if(flock(fd, LOCK_EX | LOCK_NB) < 0){
			//使用中
			close(fd);
			continue;
		}

		//確保完了
		return fd;
	}

	//なかった
	return -1;
}






/////VIEW関連

extern "C"{
	extern char _binary_rift_vertex_glsl_start[];
	extern char _binary_rift_fragment_glsl_start[];
	extern char _binary_rift_vertex_glsl_end[];
	extern char _binary_rift_fragment_glsl_end[];
};
const char* RIFT::vertexShaderSource(_binary_rift_vertex_glsl_start);
const char* RIFT::fragmentShaderSource(_binary_rift_fragment_glsl_start);

RIFT::RIFT(int fd, unsigned w, unsigned h) :
	VIEW(w, h),
	width(w),
	height(h),
	averageRatio(1),
	correctionGain(1.0/averageRatio),
	gravity((const double[]){ 0.0, -G, 0.0 }),
	magMax((const double[]){ -MaxFloat, -MaxFloat, -MaxFloat }),
	magMin((const double[]){ MaxFloat, MaxFloat, MaxFloat }),
	vNorth((const double[]){ -1, 0, 0 }),
	magReady(false),
	magneticField((const double[3]){ 0.0, 0.0, 0.01 }),
	fd(fd){

	//過去の磁化情報があれば取得
	settings.Fetch("magMax", &magMax);
	settings.Fetch("magMin", &magMin);
	settings.Fetch("vNorth", &vNorth);
	printf("magx:%lf->%lf.\n", magMin[0], magMax[0]);
	printf("magy:%lf->%lf.\n", magMin[1], magMax[1]);
	printf("magz:%lf->%lf.\n", magMin[2], magMax[2]);

	//スケジューリングポリシーを設定
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

	//Keepaliveスレッド開始
	pthread_create(&keepaliveThread, &attr, KeepaliveThread, (void*)this);

	//センサデータ取得開始
	pthread_create(&sensorThread, &attr, _SensorThread, (void*)this);


	if(GLEW_OK != glewInit()){
		throw "GLEWが使えません";
	}
	//シェーダーコードの整形
	_binary_rift_fragment_glsl_end[-1] =
	_binary_rift_vertex_glsl_end[-1] = 0;

	//プログラマブルシェーダの設定
	GLuint vShader(glCreateShader(GL_VERTEX_SHADER));
	GLuint fShader(glCreateShader(GL_FRAGMENT_SHADER));

	if(!vShader || !fShader){
		throw "シェーダの確保に失敗";
	}

	glShaderSource(vShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vShader);

	assert(glGetError() == GL_NO_ERROR);

	glShaderSource(fShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fShader);

	assert(glGetError() == GL_NO_ERROR);

	deDistorShaderProgram = glCreateProgram();
	glAttachShader(deDistorShaderProgram, vShader);
	glAttachShader(deDistorShaderProgram, fShader);

	assert(glGetError() == GL_NO_ERROR);

	GLint linked;
	glLinkProgram(deDistorShaderProgram);
	glGetProgramiv(deDistorShaderProgram, GL_LINK_STATUS, &linked);
	if(GL_FALSE == linked){
		throw "シェーダのリンクに失敗";
	}
	assert(glGetError() == GL_NO_ERROR);

	glUseProgram(0);

	//フレームバッファ用テクスチャを確保
	glGenTextures(1, &framebufferTexture);
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(
		GL_TEXTURE_2D,
		GL_TEXTURE_BORDER_COLOR,
		(const float[]){ 0, 0, 0, 1 });
	assert(glGetError() == GL_NO_ERROR);
}


RIFT::~RIFT(){
	pthread_cancel(sensorThread);
	pthread_join(sensorThread, 0);
	pthread_cancel(keepaliveThread);
	pthread_join(keepaliveThread, 0);
	close(fd);

	//磁化情報を保存
	settings.Store("magMax", &magMax);
	settings.Store("magMin", &magMin);
	settings.Store("vNorth", &vNorth);
}


void RIFT::DeDistore(){
	//Riftの歪み除去
	glGetError();
	glViewport(0, 0, width, height);
	assert(glGetError() == GL_NO_ERROR);
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	assert(glGetError() == GL_NO_ERROR);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, width, height, 0);
	assert(glGetError() == GL_NO_ERROR);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);

	//フラグメントシェーダによる歪み除去
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, deDistorTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	glUseProgram(deDistorShaderProgram);
	glUniform1i(glGetUniformLocation(deDistorShaderProgram, "buffer"), 0);
	glUniform1i(glGetUniformLocation(deDistorShaderProgram, "de_distor"), 1);
	//視野いっぱいにフレームバッファテクスチャを貼り付ける
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(0, 0); glVertex3f(-1, -1, 0.5);
	glTexCoord2f(0, 1); glVertex3f(-1, 1, 0.5);
	glTexCoord2f(1, 0); glVertex3f(1, -1, 0.5);
	glTexCoord2f(1, 1); glVertex3f(1, 1, 0.5);
	glEnd();
	glUseProgram(0);
	assert(glGetError() == GL_NO_ERROR);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, 0);
}





float RIFT::D(float dd){
	return 1.0 +
	0.35 * dd +
	0.125 * dd*dd +
	0.075 * dd*dd*dd;
}

RIFT::P2 RIFT::GetTrueCoord(float u, float v){
	const P2 lens = { (1 + inset) * width/4, (float)height / 2 };

	//レンズ位置からの相対座標へ変換
	const P2 l = { u - lens.u, v - lens.v };

	//正規化された距離の自乗
	const float dd((l.u*l.u + l.v*l.v)/(lens.u*lens.u));

	//変換された距離
	const float ddd(D(dd)/* / D(1)*/);

	//結果格納
	const P2 tc = { lens.u + l.u * ddd, lens.v + l.v * ddd };

	return tc;
}



void RIFT::RegisterDeDistoreCoords(const DISTORE_ELEMENT* body){
	glGenTextures(1, &deDistorTexture);
	glBindTexture(GL_TEXTURE_2D, deDistorTexture);
	glTexParameteri(
		GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(
		GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	assert(glGetError() == GL_NO_ERROR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RG16F,
	      width, height, 0, GL_RG, GL_FLOAT, body);
	assert(glGetError() == GL_NO_ERROR);
	glBindTexture(GL_TEXTURE_2D, 0);
}


