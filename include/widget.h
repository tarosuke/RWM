/** Widget
 * ウィジェット系クラスのインターフェイス
 * 窓グループ、窓、子窓、コントロールなどはこれを導出する
 * Xの窓やVNC画面は単一のWidgetで。
 */

#pragma once

#include <event.h>
#include <view.h>
#include "../toolbox/container/list.h"


class Widget : public wO::List<Widget>::Node{
public:
	//描画系
	virtual void Update(){};
	virtual void Draw()=0;
	virtual void DrawTransparent()=0;

	//イベント系
protected:
private:
	wO::List<Widget> children;
};

