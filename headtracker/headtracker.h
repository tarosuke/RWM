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
	const QON& GetDirection() const{ return direction; };
	const VQON& GetPosition() const{
		return displacement;
	};
	void ResetAzimuth(double ratio = 1.0);
protected:
	void Rotate(const QON& r){ direction *= r; };
	void RotateAzimuth(const QON& r);
	void MoveTo(const VQON& d){ displacement = d; };
private:
	QON direction;
	VQON displacement;
};


#endif
