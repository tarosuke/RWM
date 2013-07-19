/********************************************* rift avatar controler:riftghost
 *
 */
#ifndef _RIFTGHOST_
#define _RIFTGHOST_

#include <ghost/userGhost.h>
#include <rift/rift.h>



class RIFTGHOST : public USERGHOST{
public:
	const QON& GetHead() const;
	const QON PickHeadHorizDir(float ratio);
private:
	RIFT rift;
};


#endif

