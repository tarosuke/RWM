/***************************************************** view with rift:riftView
 *
 */
#ifndef _RIFTVIEW_
#define _RIFTVIEW_

#include "view.h"
#include <rift/rift.h>

class RIFTVIEW : public VIEW{
public:
	RIFTVIEW(class AVATAR&);
	~RIFTVIEW();
	void Draw() const;
private:
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
	static const float inset;
	static const float drawingInset;
};

#endif
