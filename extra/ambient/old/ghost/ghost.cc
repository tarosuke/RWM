/********************************************************************* 空ghost
 * 何もしないghost
 */
#include "ghost.h"

#include <toolbox/qon/qon.h>


const QON& GHOST::GetHead() const{
	static QON q;
	return q;
}

const QON GHOST::PickHeadHorizDir(float){
	QON q;
	return q;
}

float GHOST::GetRotate(){ return 0.0; }
float GHOST::GetForwardStep(){ return 0.0; }
float GHOST::GetSideStep(){ return 0.0; }
unsigned GHOST::GetActions(){ return 0; }

void GHOST::Update(const class AVATAR&){}
