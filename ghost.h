/********************************************* abstract avatar controler:ghost
 *
 */
#ifndef _GHOST_
#define _GHOST_


class GHOST{
public:
	virtual const class QON& GetHead() const=0;
	virtual const QON PickHeadHorizDir(float ratio)=0;
};



#endif

