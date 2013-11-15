/******************************************************** adstract headtracker
 *
 */

#include <math.h>

#include "headtracker.h"


//方位角リセット
void HEADTRACKER::ResetAzimuth(double ratio){
	QON o(GetDirection());
	o.i = o.k = 0.0; //垂直軸だけを残す
	o.Normalize();
	o *= ratio;
	offset *= o;
}
