/***************************************************** view with rift:riftView
 *
 */
#ifndef _RIFTVIEW_
#define _RIFTVIEW_

#include "view.h"
#include <rift/rift.h>

class RIFTVIEW : public VIEW{
public:
	RIFTVIEW(class AVATAR&);
	~RIFTVIEW();
	void Draw() const;
	void Update(float);
private:
};

#endif
