/********************************************** abstract object in room:QBJECT
 *
 */
#include "object.h"
#include "room.h"


OBJECT::OBJECT(ROOM& into) : node(*this){
	into.AddObject(node);
}

OBJECT::~OBJECT(){}

PANEL::PANEL(ROOM& into) : node(*this){
	into.AddPanel(node);
}

PANEL::~PANEL(){}

GATE::GATE(ROOM& into) : node(*this){
	into.AddGate(node);
}

GATE::~GATE(){}

