#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include "view/view.h"
#include "view/skybox.h"


int main(int argc, char *argv[]){
	try{
		VIEW& v(VIEW::New());
// 		SKYBOX skybox("/home/tarosuke/pics/rwm/Above_The_Sea-small.tga");
		SKYBOX skybox("/home/tarosuke/pics/rwm/Cubic30.tga");
// 		SKYBOX skybox("/home/tarosuke/pics/rwm/image05.tga");
		v.Run();
		delete &v;
	}
	catch(const char* m){
		printf("エラー：%s.\n", m);
	}
	return 0;
}
