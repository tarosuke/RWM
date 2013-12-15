/************************************************************* gdbm wrapper:db
 *
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "db.h"


DB::DB(const char* path) throw(gdbm_error) : replaceable(true){
	try{
		db = gdbm_open(
			const_cast<char*>(path),
			512, GDBM_WRCREAT, 0600, 0);
		if(!db){
			throw gdbm_errno;
		}
	}
	catch(void*){}
}

void DB::Store(const char* key_, const void* content_, unsigned len){
	datum key = { const_cast<char*>(key_), (int)(strlen(key_) + 1) };
	datum content = { (char*)const_cast<void*>(content_), (int)len };
	gdbm_store(db, key, content, GDBM_REPLACE);
}

void DB::Fetch(const char* key_, void* content_, unsigned len){
	datum key = { const_cast<char*>(key_), (int)(strlen(key_) + 1) };
	datum content(gdbm_fetch(db, key));
	memcpy(
		content_, content.dptr,
		(int)len < content.dsize ? (int)len : content.dsize);
	free(content.dptr);
}


template<typename T> void DB::Store(const char* key, const T* body){
	Store(key, body, sizeof(T));
}
template<> void DB::Store(const char* key, const char* body){
	Store(key, body, strlen(body) + 1);
}

template<typename T> void DB::Fetch(const char* key, T* body, unsigned maxLen){
}

template<> void DB::Fetch(const char* key, const char* body, unsigned maxLen){
}
