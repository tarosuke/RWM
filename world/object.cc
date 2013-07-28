/********************************************** abstract object in room:QBJECT
 *
 */
#include "object.h"
#include "room.h"
#include "world.h"


OBJECT::OBJECT(const WORLD& world) : radius(0.3, 0, 0.3), node(*this){
	const WORLD::ENTRY ep(world.GetEntry());
	in = ep.room;
	position += ep.position;
	(*in).AddObject(node);
}

OBJECT::~OBJECT(){}

void OBJECT::MoveTo(ROOM& to){
	in = &to;
	to.AddObject(node);
}



PANEL::PANEL(ROOM& into) : node(*this){
	into.AddPanel(node);
}

PANEL::~PANEL(){}

GATE::GATE(ROOM& into) : node(*this){
	into.AddGate(node);
}

GATE::~GATE(){}

