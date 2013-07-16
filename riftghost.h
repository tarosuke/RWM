/********************************************* rift avatar controler:riftghost
 *
 */
#ifndef _RIFTGHOST_
#define _RIFTGHOST_

#include <ghost.h>
#include <rift/rift.h>



class RIFTGHOST : public GHOST{
public:
	const QON& GetHead() const;
	const QON PickHeadHorizDir(float ratio);
private:
	RIFT rift;
};


#endif

