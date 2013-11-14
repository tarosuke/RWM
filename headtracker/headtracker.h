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
protected:
	void Rotate(const QON& r){ direction *= r; };
private:
	QON direction;
};


#endif
