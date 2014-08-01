/** 抽象VIEW
 * ヘッドトラック込みの画面描画制御クラス
 * Ambientの描画など共通の処理はVIEWで行うが、デバイス固有の処理は子クラスで行う
 */
#pragma once

#include "tools/factory.h"



class VIEW{
public:

	//環境をチェックして適切なViewを返す。引数は画面サイズ
	static VIEW& New() throw(const char*);


	virtual ~VIEW(){};

protected:
	//共通描画処理の前後に呼ばれるデバイス固有処理のハンドラ
	virtual void PreDraw()=0; //投影行列の設定やディスプレイリストの設定など
	virtual void PostDraw(){}; //反対の目の分の設定、描画など

private:
};


