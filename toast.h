/****************************************************** temporary window:toast
 * Just "new TOAST("message", time=xxx)". It will be appeared and gone.
 */
#ifndef _TOAST_
#define _TOAST_

#include <window.h>

class TOAST : public WINDOW{
	TOAST();
public:
	TOAST(const char* str, unsigned ms = 3000);
private:
	~TOAST();
};



#endif
