/************************************************************ 部屋の壁とか：panel
 *
 */
#ifndef _PANEL_
#define _PANEL_

#include <toolbox/queue/queue.h>

class PANEL{
	PANEL();
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
	PANEL(class ROOM&, const PROFILE&);
	virtual ~PANEL();
	virtual void Draw(const class TEXTURE&);
	virtual void Move();
private:
	TOOLBOX::NODE<PANEL> panelNode;
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
