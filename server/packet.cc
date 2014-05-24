/////サーバ側

#include "../util/packet.h"

#include <stdio.h>
#include <stdlib.h>



namespace PACKET{

	//受信処理＆PACKETシリーズのファクトリ
	void PACKET::Receive(SOCKET& s){
		//ヘッダ受信
		static HEADER head;
		s.Receive(&head, sizeof(head));

		//ヘッダ解釈＆実体確保
		PACKET* p(0);
		switch(head.type){
		case FeatureCheck:
			p = new FEATURECHECK(s, head);
			break;
		case FeatureAnswer:
			p = new FEATUREANSWER(s, head);
			break;
		}

		if(p){
			//実行
			if((*p).Do(s)){
				//trueが返ったらdelete
				delete p;
			}
		}
	}

}

