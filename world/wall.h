/************************************************************ 部屋の壁とか：WALL
 *
 */
#ifndef _WALL_
#define _WALL_

#include "object.h"



class WALL : public PANEL{
	WALL();
public:
	struct PROFILE{
		struct POINT{
			float x;
			float y;
		}p0, p1, texOffset, texSize;
		unsigned textureID;
		float floorHeight;
		float ceilHeight;
	};
	WALL(class ROOM&, const PROFILE&);
	virtual ~WALL();
	void Draw(const class TEXSET&);
	void Collision(class OBJECT&);
private:
	const PROFILE profile;
	const float dx;
	const float dz;
	const float norm;
	const float dnx;
	const float dnz;
	const float txu0;
	const float txu1;
	const float txv0;
	const float txv1;
};

#endif
