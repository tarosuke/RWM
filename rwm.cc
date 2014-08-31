#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include "view/view.h"
#include "view/skybox.h"
#include "settings/settings.h"


int main(int argc, char *argv[]){
	//オプションの変数
	const char* skyboxKey("defaultSkybox");
	const char* skyboxPath(0);

	//コマンドライン解釈
	for(unsigned n(1); n < argc; ++n){
		const char* const arg(argv[n]);
		if('-' != *arg){
			printf("オプション：%s は解釈できない。\n", arg);
			return -1;
		}
		switch(arg[1]){
		case 'S' :
			skyboxPath = &arg[2];
			break;
		default :
			printf("オプション：%s は解釈できない。\n", arg);
			return -1;
			break;
		}
	}

	//設定ファイル読み込み
	if(!skyboxPath){
		static char sbp[256];
		settings.Fetch(skyboxKey, sbp, 255);
		sbp[255] = 0;
		skyboxPath = sbp;
	}

	//wOCE本体
	try{
		VIEW& v(VIEW::New());
		SKYBOX* sb(SKYBOX::New(skyboxPath));
		v.Run();
		if(sb){
			delete sb;
			//天箱設定を保存
			settings.Store(skyboxKey, skyboxPath, 255);
		}
		delete &v;
	}
	catch(const char* m){
		printf("エラー：%s.\n", m);
	}
	return 0;
}
