/********************************************* rift avatar controler:riftghost
 *
 */
#include <riftghost.h>


const QON& RIFTGHOST::GetHead() const{
	return rift.GetDirection();
}

const QON RIFTGHOST::PickHeadHorizDir(float ratio){
	return rift.PickHorizonal(ratio);
}
