/************************************************************* each space:ROOM
 *
 */
#ifndef _ROOM_
#define _ROOM_

#include <toolbox/queue/queue.h>


class OBJECT;
class PANEL;
class GATE;
class AVATAR;
class VQON;
class TEXSET;
class WORLD;


class ROOM{
public:
	ROOM(class WORLD& into);
	~ROOM();
	//条項更新
	void Draw(unsigned remain) const;
	void Update(float dt);
	//オブジェクト登録
	void AddGate(TOOLBOX::NODE<GATE>& node){
		gates.Add(node);
	};
	void AddPanel(TOOLBOX::NODE<PANEL>& node){
		panels.Add(node);
	};
	void AddObject(TOOLBOX::NODE<OBJECT>& node) const{
		objects.Add(node);
	};
private:
	TOOLBOX::NODE<ROOM> node;
	mutable TOOLBOX::QUEUE<GATE> gates;
	mutable TOOLBOX::QUEUE<PANEL> panels;
	mutable TOOLBOX::QUEUE<OBJECT> objects;
	WORLD& world;
	const TEXSET& texSet;
	mutable bool drawing;
};

#endif
