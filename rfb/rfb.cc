/******************************************************** RFB protocol handler
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "rfb.h"


const char* const RFB::versionString = "RFB 003.003\n";
const char* const RFB::remoteName = "rift-vnc";


///// 初期化と後始末

RFB::RFB(UPDATERECEIVER& receiver_) :
	name(0),
	receiver(receiver_),
	pixFormat(0),
	buffer(0),
	sock(socket(AF_UNIX, SOCK_STREAM, 0)),
	sequencer(&RFB::RFBProtocolVersion){

	//サーバへ接続
	printf("start to connet to unix:%s...", remoteName);
	sockaddr_un remote;
	remote.sun_family = AF_UNIX;
	strcpy(remote.sun_path, remoteName);
	const size_t len(strlen(remote.sun_path) + sizeof(remote.sun_family));
	if(connect(sock, (sockaddr*)&remote, len) < 0){
		perror("connect to vncserver");
	}
	puts("OK.");

	//スレッドを起こしてシーケンスを開始
	pthread_create(&RFBThread, NULL, RFB::RFBEntry, (void*)this);
};

RFB::~RFB(){
	if(0 <= sock){
		pthread_cancel(RFBThread);
		close(sock);
	}
	if(name){
		free(name);
	}
	if(buffer){
		free(buffer);
	}
}


void* RFB::RFBEntry(void* o){
	//致命的エラーがなければ繰り返し実行
	try{
		for(;;){
			(*(RFB*)o).Seq();
		}
	}
	catch(const char* message){
		//エラーメッセージ形式の例外
		fprintf(stderr, "error:%s.\n", message);
	}
	catch(...){ //pocketmonster exception :-P
	}

	//TODO:エラー時の処理
	return 0;
}




/////送受信サービス
void RFB::Read(void* buff, int size){
	const int result(read(sock, buff, size));
	if(result < 0 || size < result){
		throw result;
	}
}
void* RFB::Read(int size){
	void* buff(malloc(size));
	if(size){
		try{
			Read(buff, size);
		}
		catch(int result){
			free(buff);
			throw result;
		}
	}else{
		throw errno;
	}
	return buff;
}
void RFB::Write(const void* buff, int size){
	const int result(write(sock, buff, size));
	if(result < 0 || size < result){
		throw result;
	}
}




/////プロトコルシーケンサ

void RFB::RFBProtocolVersion(){
	//版の受信(こちらは3.3なのでチェックせずに捨てる)
	const size_t vLen(12);
	char buff[vLen + 1];
	Read(buff, vLen);

	//受信内容チェック
	if(strncmp(versionString, buff, 4)){
		//受信内容が異常
		throw;
	}

	//こちらの版を送る
	Write(versionString, vLen);

	//シーケンスを次(セキュリティネゴシエーション)に進める
	sequencer = &RFB::RFBSecurity;
}

void RFB::RFBSecurity(){
	char secType;
	Read(&secType, 1);

	if(1 != secType){
		//認証はサポートしていない
		throw "no authentication supported";
	}

	//認証なしを送信
	Write(&secType, 1);

	//ClientInitをやっておく(共有可能を指定)
	Write(&secType, 1);

	//シーケンスを進める
	sequencer = &RFB::RFBServerInit;
}


const RFB::PIXFORMAT RFB::formats[] = {
	{ 32, 255, 0, 1, 0xff00, 0xff00, 0xff00, 24, 16, 8 }, //RGBZ
	{ 32, 255, 0, 1, 0xff00, 0xff00, 0xff00, 8, 16, 24 }, //BGRZ
	{ 32, 255, 0, 1, 0xff00, 0xff00, 0xff00, 16, 8, 0 }, //ZRGB
	{ 32, 255, 0, 1, 0xff00, 0xff00, 0xff00, 0, 8, 16 }, //ZRGB
	{ 0 } //終端
};

void RFB::RFBServerInit(){
	struct PACK{
		unsigned short width;
		unsigned short height;
		PIXFORMAT format;
		unsigned nameLen;
	}__attribute__((packed)) pack;

	//パケット受信
	Read((void*)&pack, sizeof(pack));

	//画面名を取得(正直いらねー)
	name = (char*)Read(pack.nameLen);

	//値を保存
	width = pack.width;
	height = pack.height;

	//必要ならピクセルフォーマットを要求
	const PIXFORMAT& p(pack.format);
	for(int i(0); formats[i].bpp; i++){
		const PIXFORMAT& f(formats[i]);
		if(f.bpp == p.bpp &&
		   !f.bigEndian == !p.bigEndian &&
		   !f.trueColor == !p.trueColor &&
		   f.maxRed == p.maxRed &&
		   f.maxGreen == p.maxGreen &&
		   f.maxBlue == p.maxBlue &&
		   f.shiftRed == p.shiftRed &&
		   f.shiftGreen == p.shiftGreen &&
		   f.shiftBlue == p.shiftBlue){
			//一致
			pixFormat = &f;
			break;
		}
	}
	if(!pixFormat){
		const PIXFORMAT f(pixFormat[0]);

		//一致なしなのでフォーマットを要求
		char buff[2];
		buff[0] = 0;
		Write((void*)buff, 2);
		Write((void*)&f, sizeof(PIXFORMAT));

		//ピクセルフォーマットを記録
		pixFormat = &f;
	}

	//TODO:エンコード要求

	//TODO:初期バッファ確保

	//TODO:フレームバッファ全体を要求

	//次のシーケンスへ
// 	sequencer = RFBUpdateRequest;
}


