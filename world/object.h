/********************************************** abstract object in room:QBJECT
 *
 */
#ifndef _OBJECT_
#define _OBJECT_

#include <toolbox/queue/queue.h>


/***** 動く物体
 *
 */
class OBJECT{
	OBJECT();
public:
	OBJECT(class ROOM& into);
	virtual ~OBJECT();
	virtual void Collision(OBJECT&)=0;
	virtual void Update(float dt)=0;
	virtual void Draw(const class TEXSET&)=0;
private:
	TOOLBOX::NODE<OBJECT> node;
};

/***** 動かない物体
 *
 */
class PANEL{
	PANEL();
public:
	PANEL(class ROOM& into);
	virtual ~PANEL();
	virtual void Collision(OBJECT&)=0;
	virtual void Draw(const class TEXSET&)=0;
private:
	TOOLBOX::NODE<PANEL> node;
};

/***** 他の部屋への通路
 *
 */
class GATE{
	GATE();
public:
	GATE(class ROOM& into);
	virtual ~GATE();
	virtual void Collision(GATE&)=0;
	virtual void Draw(unsigned remain, const class TEXSET&)=0;
private:
	TOOLBOX::NODE<GATE> node;
};

#endif
