/********************************************** abstract object in room:QBJECT
 *
 */
#ifndef _OBJECT_
#define _OBJECT_

#include <toolbox/qon//qon.h>
#include <toolbox/queue/queue.h>


/***** 動く物体
 *
 */
class OBJECT{
	OBJECT();
public:
	OBJECT(const class WORLD& world); //TODO:半径の設定
	virtual ~OBJECT();
	virtual void Update(float dt)=0;
	virtual void Draw(const class TEXSET&) const =0;
	//衝突処理関連
	virtual void Collision(){}; //TODO:OBJECT同士の衝突処理
	const VQON& Position() const { return position; };
	const VQON& Velocity() const { return velocity; };
	const VQON& Radius() const { return radius; };
	void Accel(const VQON& accel){ //物体を加速して衝突の結果を反映する
		velocity += accel;
		position += accel;
	};
	virtual void MoveTo(class ROOM&); //部屋を移動
	class ROOM& RoomIn(){ return *in; };
protected:
	class ROOM* in;
	//位置や速度
	VQON position;
	VQON velocity;
	VQON radius;
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
	virtual void Draw(const class TEXSET&)=0;
	virtual void Collision(OBJECT&)=0;
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
	virtual void Draw(unsigned remain, const class TEXSET&)=0;
	virtual void Collision(OBJECT&)=0;
private:
	TOOLBOX::NODE<GATE> node;
};

#endif
