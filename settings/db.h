/************************************************************* gdbm wrapper:db
 * datamun型は、dptrとdsizeのメンバを持つ
 */

#ifndef _DB_
#define _DB_


#include <gdbm/gdbm.h>


class DB{
public:
	DB(const char* path);
	~DB(){ if(db){ gdbm_close(db); } };
	bool IsAvail(){ return !!db; };
	operator bool(){ return IsAvail(); };
	template<typename T> void Store(const char* key, const T* body){
		Store_(key, body, sizeof(T));
	};
	template<typename T> void Store(const char* key, const T* body, unsigned len){
		Store_(key, body, len);
	};
	template<typename T> bool Fetch(
		const char* key, T* body, unsigned maxLen = 0){
		return Fetch_(key, body, sizeof(T));
	};

	void Sync(){ if(db){ gdbm_sync(db); } };
	void SetReplaceable(bool value){
		replaceable = value;
	};
private:
	GDBM_FILE db;
	bool replaceable;

	void Store_(const char* key, const void* content, unsigned len);
	bool Fetch_(const char* key, void* content, unsigned len);
};

#endif
