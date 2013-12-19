/******************************************************** adstract headtracker
 *
 */
#ifndef _HEADTRACKER_
#define _HEADTRACKER_

#include <toolbox/qon/qon.h>


class HEADTRACKER{
public:
	HEADTRACKER(){};
	~HEADTRACKER(){};
	const QON GetDirection() const{
		QON r(-offset);
		r *= direction;
		return r; };
	void ResetAzimuth(double ratio = 1.0);
protected:
	void Rotate(const QON& r){ direction *= r; };
	void RotateAzimuth(const QON& r){ offset *= r; };
private:
	QON direction;
	QON offset;
};


#endif
