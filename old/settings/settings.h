/********************************************************** rwm settings store
 *
 */

#ifndef _SETTINGS_
#define _SETTINGS_


#include "db.h"

class SETTINGS : public DB{
public:
	SETTINGS();

private:
	static char* path;
	static const char* GetPath();
};

extern SETTINGS settings;


#endif
