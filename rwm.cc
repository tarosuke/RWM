#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include "view/view.h"
#include "view/skybox.h"
#include "rift/rift.h"
#include "settings/settings.h"
#include "x/xDisplay.h"


int main(int argc, char *argv[]){
	//オプションの変数
	const char* skyboxKey("defaultSkybox");
	const char* skyboxPath(0);
	bool noSkybox(false);

	//コマンドライン解釈
	for(unsigned n(1); n < argc; ++n){
		const char* const arg(argv[n]);
		if('-' != *arg){
			printf("オプション：%s は解釈できない。\n", arg);
			return -1;
		}
		switch(arg[1]){
		case 's' :
			//天箱なし
			noSkybox = true;
			break;
		case 'S' :
			//天箱指定
			skyboxPath = &arg[2];
			break;
		case 'M' :
			//磁化情報削除
			RIFT::noReadMag = true;
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
		XDISPLAY* xd(new XSLAVEDISPLAY(":15"));
		SKYBOX* sb(0);
		if(!noSkybox && skyboxPath){
			sb = SKYBOX::New(skyboxPath);
		}
		v.Run();
		if(sb){
			delete sb;
			//天箱設定を保存
			settings.Store(skyboxKey, skyboxPath, 255);
		}
		delete &v;
		if(xd){ delete xd; };
	}
	catch(const char* m){
		printf("エラー：%s.\n", m);
	}
	return 0;
}
