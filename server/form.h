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
	FORM(SOCKET&, const HEADER&);
	~FORM();
	static void DrawAll(); //不透過部分を描画
	static void DrawTransparents(); //透過部分を逆順に
private:
	struct VERTEX{
		struct{
			float x;
			float y;
			float z;
		}__attribute__((packed)) vertex, normal;
		struct{
			float r;
			float g;
			float b;
			float a;
		}__attribute__((packed)) color;
		struct{
			float u;
			float v;
		}__attribute__((packed)) texCoord;
	}__attribute__((packed));
	struct PRIMITIVE{
		unsigned start;
		unsigned count;
	}__attribute__((packed));
	struct ROOT{
		unsigned type;
		unsigned textureID;
		unsigned numOfPrimitives;
		unsigned numOfVertexes;
		PRIMITIVE primitives[0];
	}__attribute__((packed));

	//形状関連
	static const FORM* forms; //形状情報の配列(読み込み完了後に有効)
	static TOOLBOX::QUEUE<FORM> stocks;
	static TOOLBOX::QUEUE<FORM> drawTargets;
	TOOLBOX::NODE<FORM> stockNode; //ストックのためのノード
	TOOLBOX::NODE<FORM> drawNode; //描画対象リストのためのノード

	//描画内容
	unsigned vertexBufferID; //VBOのID
	unsigned drawMode; //描画するプリミティブの種類
	const int* startIndex; //各図形の最初のインデクス番号
	const int* counts; //各図形の頂点数
	unsigned numOfPrimitives; //図形の数

	virtual void Draw();



	//パケット関連
	bool Do(SOCKET&);
};

