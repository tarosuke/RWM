/********************************************************** rwm settings store
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "settings.h"


char* SETTINGS::path(0);

SETTINGS::SETTINGS() : DB(GetPath()){
	if(path){
		free(path);
	}
	path = 0;
}

const char* SETTINGS::GetPath(){
	const char* fileName = ".rwm.settings";
	const char* home(getenv("HOME"));
	path = (char*)malloc(strlen(home) + strlen(fileName) + 2);
	sprintf(path, "%s/%s", home, fileName);

	return path;
}

SETTINGS settings;
