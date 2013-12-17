#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include <view/riftView.h>
#include <window.h>


int main(int argc, char *argv[]){
	static RIFTVIEW view;
	view.Run();

	return 0;
}
