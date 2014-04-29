/******************************************************************* GATEPLANE
 * 他の部屋への穴(垂直)
 */
#ifndef _GATEPLANE_
#define _GATEPLANE_

#include "object.h"
#include "world.h"


class GATEPLANE : public GATE{
	GATEPLANE();
public:
	struct PROFILE{
		struct POINT{
			float x;
			float y;
		}points[6];
		unsigned toRoom;
		unsigned numOfCorners;
		float height;
		bool floor; //床と天井では方向が逆になるので区別する
		bool thouable;
	};
	GATEPLANE(WORLD&, class ROOM&, const PROFILE&);
	virtual ~GATEPLANE();
	void Draw(unsigned remain, const class TEXSET&);
	bool Collision(class OBJECT&);
private:
	const PROFILE profile;
	const float normaly;
	const unsigned face;
	WORLD::GATE to;
};

#endif



