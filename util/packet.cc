#include "packet.h"

#include <stdio.h>


namespace PACKET{

	unsigned short PACKET::sequence(0);

	PACKET::PACKET(TYPE type, unsigned short size, void* buff) :
		body(buff),
		length(size){
		header.type = (unsigned short)type;
		header.length = size;
		header.sequence = ++sequence;
		header._reserved = 0;
	}

	bool PACKET::SendTo(SOCKET& s){
		//ヘッダ送信
		s.Send(&header, sizeof(header));
		//ボディ
		if(length && body){
			s.Send(body, length);
		}
		return true;
	}



	//受信処理＆PACKETシリーズのファクトリ
	void PACKET::Receive(SOCKET& s){
		//ヘッダ受信
		static HEADER head;
		s.Receive(&head, sizeof(head));

		//ヘッダ解釈＆実体確保
		PACKET* p(0);
		switch(head.type){
		case FeatureCheck:
			p = new FEATURECHECK(head);
			break;
		case FeatureAnswer:
			p = new FEATUREANSWER(head);
			break;
		}

		if(p){
			//内容の読み込み(あれば)
			if((*p).length){
				s.Receive((*p).body, (*p).length);
			}

			//実行
			(*p).Do(s);

			//領域破棄
			delete p;
		}
	}

}

