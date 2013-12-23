/******************************************************** adstract headtracker
 *
 */

#include <math.h>

#include "headtracker.h"


//方位角リセット
void HEADTRACKER::ResetAzimuth(double ratio){
	QON o(GetDirection());
	o.FilterAxis(2); //垂直軸だけを残す
	o *= ratio;
	offset *= o;
}
