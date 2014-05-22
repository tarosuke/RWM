///// パケット共通コード


#include "../util/packet.h"

#include <stdio.h>
#include <stdlib.h>


namespace PACKET{

	unsigned short PACKET::sequence(0);

	//送信用
	PACKET::PACKET(TYPE type, unsigned short size, void* buff) :
		header((const HEADER){ type, size, sequence++, 0 }),
		body(buff),
		mallocedBody(false){}

	//受信用
	PACKET::PACKET(SOCKET& s, const HEADER& h) :
		header(h),
		body(h.length ? malloc(h.length) : 0),
		mallocedBody(true){
		if(body){ //TODO:malloc失敗判定、処理
			//必要ならbodyの読み込み
			s.Receive(body, h.length); //TODO:受信失敗処理
		}
	}

	PACKET::~PACKET(){
		if(body && mallocedBody){
			free(body);
		}
	}


	bool PACKET::SendTo(SOCKET& s){
		//ヘッダ送信
		s.Send(&header, sizeof(header));
		//ボディ
		if(header.length && body){
			s.Send(body, header.length);
		}
		return true;
	}
}

