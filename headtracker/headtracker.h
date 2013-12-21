/******************************************************** adstract headtracker
 *
 */
#ifndef _HEADTRACKER_
#define _HEADTRACKER_

#include <toolbox/qon/qon.h>


class HEADTRACKER{
public:
	struct STATE{
		QON direction;
		QON azimuth;
		VQON place;
	};
	HEADTRACKER(){};
	~HEADTRACKER(){};
	const QON& GetView();
	void ResetAzimuth(double ratio = 1.0);
protected:
	virtual const STATE& GetState() const{ return state; };
private:
	void Update();
	STATE state;
	QON azimuth;
};


#endif
