/*********************************************** ghost for localuser:USERGHOST
 *
 */
#ifndef _USERGHOST_
#define _USERGHOST_

#include <ghost/ghost.h>
#include <toolbox/input/joystick.h>
#include <toolbox/qon/qon.h>


class USERGHOST : public GHOST{
public:
	//頭操作
	const class QON& GetHead() const;
	const QON PickHeadHorizDir(float ratio);
	//操作取得
	float GetRotate();
	float GetForwardStep();
	float GetSideStep();
	unsigned GetActions();
protected:
	JOYSTICK js;
	mutable QON headDir;
};






#endif
