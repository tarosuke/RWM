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
		bool throuable;
	};
	GATEPANEL(WORLD& world, class ROOM& into, const PROFILE& profile);
	void Draw(unsigned remain, const class TEXSET&);
	bool Collision(class OBJECT&);
private:
	const PROFILE profile;
	WORLD::GATE to;
	const float dx; //壁の端点から端点
	const float dz;
	const float norm; //壁の長さ
	const float dnx; //正規化した壁の端点から端点
	const float dnz;
};


#endif
