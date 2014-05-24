/*************************************************************** a world:WORLD
 *
 */
#ifndef _WORLD_
#define _WORLD_

#include <toolbox/queue/queue.h>
#include <toolbox/qon//qon.h>
#include "texset.h"


class WORLD{
public:
	struct ENTRY{
		class ROOM* room;
		VQON position;
	};
	WORLD(const char* worldFile);
	const ENTRY& GetEntry() const;
	void Add(TOOLBOX::NODE<class ROOM>& room);
	class GATE{
	public:
		GATE(WORLD& world_, unsigned roomID) :
			world(world_), rID(roomID), room(0){};
		operator class ROOM*();
	private:
		WORLD& world;
		const unsigned rID;
		class ROOM* room;
	};
	const TEXSET& GetTextureSet(){
		return texSet;
	};
private:
	TOOLBOX::QUEUE<class ROOM> rooms;
	TEXSET texSet;
};



#endif
