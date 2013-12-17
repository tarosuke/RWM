/************************************************************* gdbm wrapper:db
 *
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "db.h"


DB::DB(const char* path) : replaceable(true){
	db = gdbm_open(
		const_cast<char*>(path),
		512, GDBM_WRCREAT, 0600, 0);
printf("database open %s.\n", db ? "successed" : "failed");
}

void DB::Store_(const char* key_, const void* content_, unsigned len){
	if(!IsAvail()){
		return;
	}
	datum key = { const_cast<char*>(key_), (int)(strlen(key_) + 1) };
	datum content = { (char*)const_cast<void*>(content_), (int)len };
	gdbm_store(db, key, content, GDBM_REPLACE);
}

bool DB::Fetch_(const char* key_, void* content_, unsigned len){
printf("fetch? %s, %p, %u.\n", key_, content_, len);
	if(!IsAvail()){
		return false;
	}
puts("create key.");
	datum key = { const_cast<char*>(key_), (int)(strlen(key_) + 1) };
puts("fetching.");
	datum content(gdbm_fetch(db, key));
	if(content.dptr){
printf("fetch successed: ptr:%p len:%d.\n", content.dptr, content.dsize);
		memcpy(content_, content.dptr,
			(int)len < content.dsize ? (int)len : content.dsize);
		free(content.dptr);
		return true;
	}
puts("fetch failed.");
	return false;
}


template<> void DB::Store(const char* key, const char* body){
	Store_(key, body, strlen(body) + 1);
}

template<> bool DB::Fetch(const char* key, char* body, unsigned maxLen){
	const bool stat(Fetch_(key, body, maxLen));
	body[stat ? maxLen - 1 : 0] = 0;
	return stat;
}
