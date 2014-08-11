#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include "view/view.h"


int main(int argc, char *argv[]){
	VIEW& v(VIEW::New());
	v.Run();
	return 0;
}
