#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include <view/riftView.h>
#include <window.h>
#include <server.h>


int main(int argc, char *argv[]){
	static RIFTVIEW view;
	if(SERVER::Start(view, "rwm")){
		view.Run();
	}else{
		puts("socket fail.");
	}

	return 0;
}
