/*********************************************** ghost for localuser:USERGHOST
 *
 */
#include "userGhost.h"
#include <math.h>


const QON& USERGHOST::GetHead()const{
	//TODO:jsの軸2,3で頭の向きを作る
	const float r(M_PI / 2.0);
	const float h(r * js.GetAxis(2));
	const float v(r * js.GetAxis(3));
	QON::ROTATION rot = { h, 0, 1, 0 };
	static QON hq(rot);
	rot.angle = v;
	rot.x = 1;
	rot.y = 0;
	const QON vq(rot);
	hq *= vq;
	headDir = hq;
	return headDir;
}

const QON USERGHOST::PickHeadHorizDir(float){
	QON q;
	return q;
}

float USERGHOST::GetRotate(){
	return js.GetAxis(0) * M_PI;
}

float USERGHOST::GetForwardStep(){
	const float f(-js.GetAxis(1));
	return 0 <= f ? f : f * 0.5;
}

float USERGHOST::GetSideStep(){
	const unsigned buttons(js.GetButtons());
	return (((1U << 6) & buttons) ? -1.0 : 0.0) +
		(((1U << 7) & buttons) ? 1.0 : 0.0);
}

unsigned USERGHOST::GetActions(){
	return js.GetButtons();
}


