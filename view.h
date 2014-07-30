/** 抽象VIEW
 * ヘッドトラック込みの画面描画制御クラス
 * Ambientの描画など共通の処理はVIEWで行うが、デバイス固有の処理は子クラスで行う
 */
#pragma once


class VIEW{
public:

	//環境をチェックして適切なViewを返す。引数は画面サイズ
	static VIEW& New(unsigned width, unsigned height) throw(const char*);


	virtual ~VIEW(){};

protected:

	virtual void PreDraw()=0; //投影行列の設定やディスプレイリストの設定など
	virtual void PostDraw()=0; //反対の目の分の設定、描画など

private:
};


