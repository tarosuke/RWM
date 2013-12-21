/******************************************************** adstract headtracker
 *
 */

#include <math.h>
#include <toolbox/qon/glqon.h>

#include "headtracker.h"

//方位角リセット
void HEADTRACKER::ResetAzimuth(double ratio){
	//方位の取得
	Update();
	QON o(-state.azimuth);
	o *= -azimuth;
	o *= state.direction;

	//方位の修正
	o.i = o.k = 0.0; //垂直軸だけを残す
	o.Normalize();
	o *= ratio;
	azimuth *= o;
}

//OpenGLのViewを取得
const QON& HEADTRACKER::GetView(){
	Update();
	QON dir(-state.azimuth);
	dir *= -azimuth;
	dir *= state.direction;
	GLQON glHeadDir(dir);
	glHeadDir.GetView(); //向きを反映
	GLVQON glPlace(state.place);
	glPlace.GetView(); //位置を反映
	return state.direction;
}

void HEADTRACKER::Update(){
	const STATE& s(GetState());
	state = s;
}
