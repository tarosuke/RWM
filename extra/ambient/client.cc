


#include "client.h"

#include <stdio.h>




namespace PACKET{
	void FEATURECHECK::Do(SOCKET&){}
	void FEATUREANSWER::Do(SOCKET&){
		printf("featureAnswer:%x.\n", features);
	}
}

