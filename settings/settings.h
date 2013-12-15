/********************************************************** rwm settings store
 *
 */

#ifndef _SETTINGS_
#define _SETTINGS_


#include "db.h"

class SETTINGS : private DB{
public:
	SETTINGS() : DB("rwm.settings"){};
};

extern SETTINGS settings;


#endif
