#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include "view/view.h"


int main(int argc, char *argv[]){
	try{
		VIEW& v(VIEW::New());
		v.Run();
		delete &v;
	}
	catch(const char* m){
		printf("エラー：%s.\n", m);
	}
	return 0;
}
