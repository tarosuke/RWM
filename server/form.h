///形状オブジェクト
#pragma once

#include <util/packet.h>
#include <toolbox/queue/queue.h>



///形状パケット＆データ
class FORM : public PACKET::PACKET{
	FORM();
	FORM(FORM&);
	void operator=(FORM&);
public:
	struct VERTEX{
		float x;
		float y;
		float z;
		float r;
		float g;
		float b;
		float u;
		float v;
		float nx;
		float ny;
		float nz;
	}__attribute__((packed));
	FORM(const HEADER&);
	static void DrawAll(); //不透過部分を描画
	static void DrawTransparents(); //透過部分を逆順に
private:
	//形状関連
	static const FORM* forms; //形状情報の配列(読み込み完了後に有効)
	static const VERTEX* vertexes; //頂点データ
	static TOOLBOX::QUEUE<FORM> stocks;
	static TOOLBOX::QUEUE<FORM> drawTargets;
	TOOLBOX::NODE<FORM> stockNode; //ストックのためのノード
	TOOLBOX::NODE<FORM> drawNode; //描画対象リストのためのノード
	virtual void Draw();



	//パケット関連
	void Do(SOCKET&);
};

