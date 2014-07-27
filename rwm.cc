#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include "x/xDisplay.h"


int main(int argc, char *argv[]){
	static XDisplay display;
	display.Run();

	return 0;
}
