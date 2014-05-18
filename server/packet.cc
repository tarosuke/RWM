/////サーバ側

#include "../util/packet.h"
#include "form.h"

#include <stdio.h>


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
			p = new FEATURECHECK(head);
			break;
		case FeatureAnswer:
			p = new FEATUREANSWER(head);
			break;
		case Form: //頂点データ
			p = new FORM(head);
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

