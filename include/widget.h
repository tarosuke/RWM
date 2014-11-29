/** Widget
 * ウィジェット系クラスのインターフェイス
 * 窓グループ、窓、子窓、コントロールなどはこれを導出する
 * Xの窓やVNC画面は単一のWidgetで。
 */

#pragma once

#include <event.h>
#include "../toolbox/container/list.h"
#include "../gl/texture.h"
#include "../toolbox/complex/complex.h"
#include "../toolbox/geometry/vector.h"



class Widget : public wO::List<Widget>::Node{
public:
	typedef wO::Vector<int, 2> Vector;

	Widget() : wO::List<Widget>::Node(*this), center((const int[]){0,0}){};
	virtual ~Widget(){};

	virtual void Update(const COMPLEX<4>&){};
	virtual void Draw()=0;
	virtual void DrawTransparent()=0;

	//イベント系
protected:
	//頭が向いている点(仮想画面上、毎フレーム更新)
	static Vector lookingPoint;
	static float distance;

	static double rotation;  //頭の傾き[deg]

	//中央の位置(仮想画面上)
	Vector center;
	//画面に入っているかどうか(=描画の必要性)チェック
	bool IsInsight();

	//定数
	static const float scale; //窓表示スケール[m/px]
private:
};


class LeafWidget : public Widget{
private:
	//幅と高さ(仮想画面上)
	float width;
	float height;
	//テクスチャ
	GL::TEXTURE texture;
};


class BranchWidget : public Widget{
public:
	~BranchWidget();
	void Register(Widget& w){
		children.Insert(w);
	};

	void Draw();
	void DrawTransparent();
private:
	wO::List<Widget> children;
};


//VIEWから直接呼び出されるWidget。ある意味ルート
class ViewWidget : public BranchWidget{
public:
	//描画関連
	void Update(const COMPLEX<4>&);
	void Draw();
	void DrawTransparent();

	static const float baseDistance;
	static const float motionDistance;
private:
	bool lookingFront;
};

