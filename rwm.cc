#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include <view/riftView.h>
#include <window.h>
#include <ambient/ambient.h>


int main(int argc, char *argv[]){
	static RIFTVIEW view;
	static Ambient ambient(view);
	view.Run();
	return 0;
}
