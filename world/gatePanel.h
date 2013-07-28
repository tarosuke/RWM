/******************************************************************* gatepanel
 * 他の部屋への穴
 */
#ifndef _GATEPANEL_
#define _GATEPANEL_

#include "object.h"
#include "world.h"


class GATEPANEL : public GATE{
	GATEPANEL();
public:
	struct PROFILE{
		struct POINT{
			float x;
			float z;
		}p0, p1;
		unsigned toRoom;
		float floorHeight;
		float ceilHeight;
	};
	GATEPANEL(WORLD& world, class ROOM& into, const PROFILE& profile_) :
		GATE(into),
		profile(profile_),
		to(world, profile_.toRoom){
	};
	void Draw(unsigned remain, const class TEXSET&);
	void Collision(class OBJECT&);
private:
	const PROFILE profile;
	WORLD::GATE to;
};


#endif
