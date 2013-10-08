/***************************************************** view with rift:riftView
 *
 */
#ifndef _RIFTVIEW_
#define _RIFTVIEW_

#include "view.h"
#include <headtracker/rift.h>

class RIFTVIEW : public VIEW{
public:
	RIFTVIEW();
	~RIFTVIEW();
	void PreDraw();
	void PostDraw();
private:
	//歪み計算
	static bool glewValid;
	static int deDistorShaderProgram;
	static const char* vertexShaderSource;
	static const char* fragmentShaderSource;
	struct P2{
		float u;
		float v;
	};
	P2 GetTrueCoord(float u, float v);
	static float D(float l);
	unsigned framebufferTexture;
	unsigned deDistorTexture;
	RIFT rift;

	//再描画用ハンドル
	int displayList;

	//大きさなど
	static const double inset = 0.1453;
};
#endif
