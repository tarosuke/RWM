/******************************************************** adstract headtracker
 *
 */

#include <math.h>

#include "headtracker.h"


//方位角リセット
void HEADTRACKER::ResetAzimuth(double ratio){
	QON o(-direction);
	o *= ratio;
	o.FilterAxis(2); //垂直軸だけを残す
	o *= direction;
	direction = o;
}

void HEADTRACKER::RotateAzimuth(const QON& r){
	QON o(-r);
	o *= direction;
	direction = o;
}
