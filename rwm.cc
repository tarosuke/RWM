#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include "view/view.h"
#include "view/skybox.h"


int main(int argc, char *argv[]){
	try{
		SKYBOX* sb(0);
		VIEW& v(VIEW::New());
		try{
			//天箱(skybox)
			const char* path("/home/tarosuke/pics/rwm/Cubic30.tga");
// 			const char* path("/home/tarosuke/pics/rwm/Above_The_Sea.tga");
// 			const char* path("/home/tarosuke/pics/rwm/Above_The_Sea-small.tga");
// 			const char* path("/home/tarosuke/pics/rwm/image05.tga");
			sb = new SKYBOX(path);
		}
		catch(...){
			//空箱は必須ではないので失敗してもそのまま進行
			//TODO:将来的にはtoastでエラー表示
		}
		v.Run();
		delete &v;
		if(sb){ delete sb; }
	}
	catch(const char* m){
		printf("エラー：%s.\n", m);
	}
	return 0;
}
