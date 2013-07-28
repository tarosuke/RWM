/********************************************** abstract object in room:QBJECT
 *
 */
#include "object.h"
#include "room.h"
#include "world.h"


OBJECT::OBJECT(const WORLD& world) : node(*this){
	const WORLD::ENTRY ep(world.GetEntry());
	in = ep.room;
	position += ep.position;
	(*in).AddObject(node);
}

OBJECT::~OBJECT(){}

void OBJECT::MoveTo(const ROOM& to){
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

