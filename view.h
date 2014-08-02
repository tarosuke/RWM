/** 抽象VIEW
 * ヘッドトラック込みの画面描画制御クラス
 * Ambientの描画など共通の処理はVIEWで行うが、デバイス固有の処理は子クラスで行う
 */
#pragma once

#include "toolbox/factory/factory.h"
#include "x/xDisplay.h"
#include "toolbox/queue/queue.h"
#include "toolbox/complex/complex.h"


class VIEW{
	VIEW();
	VIEW(const VIEW&);
	void operator=(const VIEW&);
public:
	//描画対象
	class DRAWER{
	public:
		//内容描画(通常/透過)
		virtual void Draw() const=0;

		//登録と解除
		void Add(TOOLBOX::QUEUE<DRAWER>& q){ node.Attach(q); };
		void Detach(){ node.Detach(); };
	protected:
		DRAWER() : node(*this){};
		virtual ~DRAWER(){};
	private:
		TOOLBOX::NODE<DRAWER> node;
	};

	//環境をチェックして適切なViewを返す。引数は画面サイズ
	static VIEW& New() throw(const char*);

	void Run();


	virtual ~VIEW(){};


	//描画オブジェクトの登録
	void RegisterStickies(DRAWER& d){ d.Add(stickeies); };
	void RegisterExternals(DRAWER& d){ d.Add(externals); };

	//描画オブジェクトの抹消(自動削除されるので普通は不要)
	void Unregister(DRAWER& d){ d.Detach(); };

protected:
	struct POSE{
		COMPLEX<4> direction;
		VECTOR<3> position;
	};

	VIEW(unsigned w, unsigned h);

	//共通描画処理の前後に呼ばれるデバイス固有処理のハンドラ
	virtual void PreDraw()=0; //投影行列の設定やディスプレイリストの設定など
	virtual void PostDraw(){}; //反対の目の分の設定、描画など
	virtual const POSE& Pose() const=0; //頭の位置と向きを反映

private:
	//X画面
	XDISPLAY xDisplay;

	//描画対象物
	TOOLBOX::QUEUE<DRAWER> stickeies;
	TOOLBOX::QUEUE<DRAWER> externals;
};


