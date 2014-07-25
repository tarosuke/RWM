/***************************************************** 床、天井などの水平面:PLANE
 *
 */
#ifndef _PLANE_
#define _PLANE_

#include "object.h"


class PLANE : public PANEL{
	PLANE();
public:
	struct PROFILE{
		struct POINT{
			float x;
			float y;
		}points[6], texOffset, texSize;
		unsigned textureID;
		unsigned numOfCorners;
		float height;
		bool floor; //床と天井では方向が逆になるので区別する
	};
	PLANE(class ROOM&, const PROFILE&);
	virtual ~PLANE();
	void Draw(const class TEXSET&);
	void Collision(class OBJECT&);
private:
	const PROFILE profile;
	const float normaly;
	const unsigned face;
};

#endif



