/** 抽象VIEW
 * ヘッドトラック込みの画面描画制御クラス
 * Ambientの描画など共通の処理はVIEWで行うが、デバイス固有の処理は子クラスで行う
 */
#pragma once

#include "../toolbox/factory/factory.h"
#include "../toolbox/queue/queue.h"
#include "../toolbox/complex/complex.h"

#include <widget.h>



class VIEW{
	VIEW();
	VIEW(const VIEW&);
	void operator=(const VIEW&);
public:
	//描画対象
	class DRAWER{
	public:
		//状況の更新
		virtual void Update(){};

		//内容描画(通常/透過)
		virtual void Draw(){};
		virtual void DrawTransparent(){};

		//登録と解除
		void Add(TOOLBOX::QUEUE<DRAWER>& q){ node.Attach(q); };
		void Detach(){ node.Detach(); };
	protected:
		DRAWER() : node(*this){};
		virtual ~DRAWER(){};
	private:
		TOOLBOX::NODE<DRAWER> node;
	};

	//XDISPLAY用
	class DISPLAY{
	public:
		virtual void Update()=0;
		virtual void Run()=0;
	protected:
		DISPLAY() : node(*this){ node.Attach(VIEW::xDisplays); };
		virtual ~DISPLAY(){};
		static void Quit(){ VIEW::Quit(); };
	private:
		TOOLBOX::NODE<DISPLAY> node;
	};

	//環境をチェックして適切なViewを返す。引数は画面サイズ
	static VIEW& New() throw(const char*);

	void Run();

	virtual ~VIEW();


	//描画オブジェクトの登録
	static void RegisterStickies(DRAWER& d){ d.Add(stickeies); };
	static void RegisterExternals(DRAWER& d){ d.Add(externals); };
	static void RegisterSkybox(DRAWER& d){ d.Add(skyboxes); };

	//描画オブジェクトの抹消(自動削除されるので普通は不要)
	void Unregister(DRAWER& d){ d.Detach(); };

	//天箱の色を設定
	void SetSkyboxColor(float r, float g, float b, float a){
		skyboxColor.r = r;
		skyboxColor.g = g;
		skyboxColor.b = b;
		skyboxColor.a = a;
	};

	//起動進捗チェック
	virtual float InitialProgress()=0;

protected:
	struct POSE{
		COMPLEX<4> direction;
		VECTOR<3> position;
	};

	VIEW(unsigned w, unsigned h);
	float GetTanFov(){ return tanFov; };

	//共通描画処理の前後に呼ばれるデバイス固有処理のハンドラ
	virtual void PreDraw()=0; //投影行列の設定やディスプレイリストの設定など
	virtual void PostDraw(){}; //反対の目の分の設定、描画など
	virtual const POSE& Pose() const=0; //頭の位置と向きを反映

private:
	class XDISPLAY* const xDisplay; //表示用画面

	//描画対象物
	static TOOLBOX::QUEUE<DRAWER> stickeies;
	static TOOLBOX::QUEUE<DRAWER> externals;
	static TOOLBOX::QUEUE<DRAWER> skyboxes;

	//X画面
	static TOOLBOX::QUEUE<DISPLAY> xDisplays;

	//視野角
	static float fov;
	static float tanFov;

	struct{
		float r;
		float g;
		float b;
		float a;
	}skyboxColor;

	class READY : public DRAWER{
	public:
		READY(VIEW&);
	private:
		static const unsigned faiFrames = 100;
		unsigned fai; //initialProgressが1になってからのフレーム数
		VIEW& view;
		void Update();
	};

	//終了？
	static bool keep;
	static void Quit(){ keep = false; };
};


