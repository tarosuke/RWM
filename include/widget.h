/** Widget
 * ウィジェット系クラスのインターフェイス
 * 窓グループ、窓、子窓、コントロールなどはこれを導出する
 * Xの窓やVNC画面は単一のWidgetで。
 */

#pragma once

#include <event.h>
#include "../toolbox/container/list.h"
#include "../gl/texture.h"


class Widget : public wO::List<Widget>::Node{
public:
	virtual void Draw()=0;
	virtual void DrawTransparent()=0;

	//イベント系
protected:
	//頭が向いている点(仮想画面上、毎フレーム更新)
	static float sightX;
	static float sightY;
	static float distance;
	//中央の位置(仮想画面上)
	float horiz;
	float vert;
private:
};


class LeafWidget : public Widget{
private:
	//幅と高さ(仮想画面上)
	float width;
	float height;
	//テクスチャ
	GL::TEXTURE texture;

	void Draw();
	void DrawTransparent();
};


class BranchWidget : public Widget{
public:
	void Draw();
	void DrawTransparent();
private:
	wO::List<Widget> children;
};


//VIEWから直接呼び出されるWidget。ある意味ルート
class ViewWidget : public BranchWidget{
public:
	//頭の向きを記録(毎フレーム更新される)
	void Update(float x, float y){
		sightX = x;
		sightY = y;
		distance = baseDistance;
	};


	static const float baseDistance;
};

