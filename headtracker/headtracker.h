/******************************************************** adstract headtracker
 *
 */
#ifndef _HEADTRACKER_
#define _HEADTRACKER_

#include <toolbox/qon/qon.h>


class HEADTRACKER{
public:
	HEADTRACKER(){};
	virtual ~HEADTRACKER(){};
	virtual const QON& GetDirection() const{ return direction; };
protected:
	QON direction;
};


#endif
