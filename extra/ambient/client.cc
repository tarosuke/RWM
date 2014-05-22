


#include "client.h"

#include <stdio.h>




namespace PACKET{
	bool FEATURECHECK::Do(SOCKET&){ return true; }
	bool FEATUREANSWER::Do(SOCKET&){
		printf("featureAnswer:%x.\n", features);
		return true;
	}
}

