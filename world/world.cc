/*************************************************************** a world:WORLD
 *
 */
#include "world.h"
#include "room.h"
#include "texset.h"
#include "wall.h"
#include "plane.h"

#include <image/ppm.h>


WORLD::WORLD(const char* worldFile){
	if(worldFile){
	}





	//ワールドファイルを読めなかったのでサンプルワールドを構築
	PPM textureImage("textureSet.ppm");
	texSet.Load(textureImage);

	ROOM& room(*new ROOM(*this));
	WALL::PROFILE wp = {
		{ -2, -3 }, { 2, -3 },
		{ 0, 0 }, { 0.3, 0.3 },
		3, 0.0, 2.4 };
	new WALL(room, wp);
	wp.p0.x = 2;
	wp.p1.y = 3;
	new WALL(room, wp);
	wp.p0.y = 3;
	wp.p1.x = -2;
	new WALL(room, wp);
	wp.p0.x = -2;
	wp.p1.y = -3;
	new WALL(room, wp);

	PLANE::PROFILE pp = {
		{ { -2, -3 }, { 2, -3 }, { 2, 3 }, { -2, 3 } },
		{ 0, 0 }, { 0.6, 0.6 },
		1, 4, 0, true };
	new PLANE(room, pp);
	pp.textureID = 2;
	pp.height = 2.4;
	pp.floor = false;
	new PLANE(room, pp);

	//天箱
	ROOM& skyBox(*new ROOM(*this));
	wp.p0.x = wp.p1.x = -1000;
	wp.p1.y = -1000;
	wp.p0.y = 1000;
	wp.texSize.x = 2000;
	wp.texSize.y = -2000;
	wp.textureID = 5;
	wp.floorHeight = -1000;
	wp.ceilHeight = 1000;
	new WALL(skyBox, wp);
	wp.p1.x = 1000;
	wp.p0.y = wp.p1.y = -1000;
	wp.textureID = 6;
	new WALL(skyBox, wp);
	wp.p0.x = wp.p1.x = wp.p1.y = 1000;
	wp.p0.y = -1000;
	wp.textureID = 7;
	new WALL(skyBox, wp);
	wp.p1.x = -1000;
	wp.p0.y = wp.p1.y = wp.p0.x = 1000;
	wp.textureID = 8;
	new WALL(skyBox, wp);

	PLANE::PROFILE bpp = {
		{ { -1000, -1000 }, { 1000, -1000 },
		{ 1000, 1000 }, { -1000, 1000 } },
		{ 0.5, 0.5 }, { 2000, -2000 },
		9, 4, 1000, false };
	new PLANE(skyBox, bpp);
	bpp.height = -1000;
	bpp.textureID = 10;
	bpp.floor = true;
	new PLANE(skyBox, bpp);
}

void WORLD::Add(TOOLBOX::NODE<ROOM>& room){
	rooms.Add(room);
}

const WORLD::ENTRY& WORLD::GetEntry() const{
	static ENTRY ep;
	ep.room = rooms.Peek();
	return ep;
}

WORLD::GATE::operator ROOM*(){
	if(!room){
		TOOLBOX::QUEUE<ROOM>::ITOR r(world.rooms);
		for(unsigned n(0); n < rID; n++, r++);
		room = r;
	}
	return room;
}

