/************************************************************* gdbm wrapper:db
 * datamun型は、dptrとdsizeのメンバを持つ
 */

#ifndef _DB_
#define _DB_


#include "gdbm.h"


class DB{
public:
	DB(const char* path) throw(gdbm_error);
	~DB(){ gdbm_close(db); };
	template<typename T> void Store(const char* key, const T* body);
	template<typename T> void Fetch(
		const char* key, T* body, unsigned maxLen = 0);

	void Sync(){ gdbm_sync(db); };
	void SetReplaceable(bool value){
		replaceable = value;
	};
private:
	GDBM_FILE db;
	bool replaceable;

	void Store(const char* key, const void* content, unsigned len);
	void Fetch(const char* key, void* content, unsigned len);
};

#endif
