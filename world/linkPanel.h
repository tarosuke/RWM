/******************************************************************* linkpanel
 * 他の部屋への穴
 */
#ifndef _LINKPANEL_
#define _LINKPANEL_

#include "room.h"


class LINKPANEL{
	LINKPANEL();
public:
	LINKPANEL(unsigned linkTo) : roomsNode(*this), link(linkTo){};
	void Draw(unsigned remain, const class TEXTURE&);
	TOOLBOX::NODE<LINKPANEL> roomsNode;
private:
 	ROOM::ROOMLINK link;
// 	const float left;
// 	const float top;
// 	const float right;
// 	const float bottom;
};


#endif

