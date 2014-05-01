/************************************************************ view(rootwindow)
 *
 */
#ifndef _VIEW_
#define _VIEW_

#include "xDisplay.h"

#include <headtracker/headtracker.h>
#include <toolbox/queue/queue.h>


class VIEW{
	VIEW();
public:
	class DRAWER{
	public:
		virtual void Draw() const=0;
		void Add(TOOLBOX::QUEUE<DRAWER>& q){
			node.Attach(q);
		};
		void Detach(){ node.Detach(); };
	protected:
		DRAWER() : node(*this){};
		virtual ~DRAWER(){};
	private:
		TOOLBOX::NODE<DRAWER> node;
	};
	VIEW(HEADTRACKER&);
	virtual ~VIEW();
	void Run();
	//描画オブジェクトの登録
	void RegisterStickies(TOOLBOX::QUEUE<DRAWER>& d);
	void RegisterExternals(TOOLBOX::QUEUE<DRAWER>& d);
	//描画オブジェクトの抹消(自動削除されるので普通は不要)
	void Unregister(DRAWER& d){ d.Detach(); };
	//終了設定
	static void Quit(){ keep = false; };
protected:
	static const float defaultDotPitch = 0.003;
	static const float defaultDisplayDistance = 0.7;
	static const float nearDistance = 0.001;
	static const float farDistance = 10000.0;

	//ヘッドトラッカー
	HEADTRACKER& head;

	//描画対象物
	class DRAWTARGETS{
	public:
		typedef TOOLBOX::QUEUE<DRAWER> Q;
		DRAWTARGETS() : now(0), next(0){};
		operator Q*() const { return now; };
		operator bool() const { return !!now; };
		void SetDrawList(const Q&);
	private:
		Q* now;
		Q* next;
	};
	DRAWTARGETS stickeies;
	DRAWTARGETS externals;

	//幅や高さ
	int width;
	int height;
	float realWidth;
	float realHeight;
	float realDistance;

	//描画ハンドラ
	XDISPLAY x;
	XVFB xvfb;
	void DrawObjects(const DRAWTARGETS&) const;
	virtual void PreDraw();
	virtual void PostDraw();
	void Update(){
		x.Update();
	};
private:
	static bool keep;
	static void SIGCHLD_Handler(int);
};


#endif
