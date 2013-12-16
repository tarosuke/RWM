/************************************************************* gdbm wrapper:db
 * datamun型は、dptrとdsizeのメンバを持つ
 */

#ifndef _DB_
#define _DB_


#include "gdbm.h"


class DB{
public:
	DB(const char* path);
	~DB(){ gdbm_close(db); };
	bool IsAvail(){ return !!db; };
	operator bool(){ return IsAvail(); };
	template<typename T> void Store(const char* key, const T* body);
	template<typename T> bool Fetch(
		const char* key, T* body, unsigned maxLen = 0);

	void Sync(){ if(db){ gdbm_sync(db); } };
	void SetReplaceable(bool value){
		replaceable = value;
	};
private:
	GDBM_FILE db;
	bool replaceable;

	void Store(const char* key, const void* content, unsigned len);
	bool Fetch(const char* key, void* content, unsigned len);
};

#endif
