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
	const VQON& GetPosition() const{
		return displacement;
	};
	void ResetAzimuth(double ratio = 1.0);
protected:
	void Rotate(const QON& r){ direction *= r; };
	void RotateAzimuth(const QON& r){ offset *= r; };
	void MoveTo(const VQON& d){ displacement = d; };
private:
	QON direction;
	QON offset;

	VQON displacement;
};


#endif
