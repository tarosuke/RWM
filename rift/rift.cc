#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <stdio.h>

#include "rift.h"


#define MaxFloat (3.40282347e+38F)
#define G (9.80665)
RIFT::RIFT(unsigned w, unsigned h) :
	VIEW(w, h),
	averageRatio(initialAverageRatio),
	gravity((const double[]){ 0.0, -G, 0.0 }),
	magMax((const double[]){ -MaxFloat, -MaxFloat, -MaxFloat }),
	magMin((const double[]){ MaxFloat, MaxFloat, MaxFloat }),
	magFront((const double[]){ 0, 0, 1 }),
	magReady(false),
	magneticField((const double[3]){ 0.0, 0.0, 0.01 }){
}













void RIFT::UpdateAngularVelocity(const int angles[3], double dt){
	COMPLEX<4> delta(angles, 0.0001 * dt);
	pose.direction *= delta;
}

void RIFT::UpdateAccelaretion(const int axis[3], double dt){
	//加速度情報取得
	VECTOR<3> acc(axis, 0.0001);

	//重力加速度分離
	VECTOR<3> g(acc);
	const double ratio(1.0 / averageRatio);
	g *= ratio;
	gravity *= 1.0 - ratio;
	gravity += g;

	//重力を除去
	acc -= gravity;

	//位置や速度を求める
	if(dt <= 0.02){ //dtが異常に大きい時はデータが欠損しているので位置更新しない
		acc.Rotate(pose.direction); //絶対座標系へ変換
		acc *= dt;
		velocity += acc;
		VECTOR<3> v(velocity);
		v *= dt;
		pose.position += v;

		//枠を出ていたら止める
		const float limit(0.4);
		double* p(pose.position);
		double* ve(velocity);
		for(unsigned n(0); n < 3; ++n){
			if(p[n] < - limit || limit < p[n]){
				ve[n] = 0;
				p[n] = p[n] < 0 ? -limit : limit;
			}
		}
		velocity *= 0.999;
		pose.position *= 0.99;
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

		//各軸ゲイン調整
		// 		double* const g(mag);
		// 		g[0] /= d[0];
		// 		g[1] /= d[1];
		// 		g[2] /= d[2];
		mag.Normalize();

		//平均化処理
		const double r(1.0 / averageRatio);
		mag *= r;
		// const double* const h(mag);
		// printf("magRatio:%u %lf %lf %lf.\n", magAverageRatio, h[1], h[2], h[3]);
		magneticField *= 1.0 - r;
		magneticField += mag;;
	}else{
		//キャリブレーション可能判定
		if(7000 < abs(d[0]) && 7000 < abs(d[1]) && 7000 < abs(d[2])){
			magReady = true;
			averageRatio = initialAverageRatio;
			puts("magnetic azimuth correction READY.");
		}
	}
}

