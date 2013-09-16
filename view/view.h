#ifndef _VIEW_
#define _VIEW_

#include <rfb/rfb.h>


class VIEW : public RFB::UPDATERECEIVER{
public:
	VIEW(int width, int height);
	virtual ~VIEW();
protected:
	static const float defaultDotPitch = 0.003;
	static const float defaultDisplayDistance = 0.7;
	static const float nearDistance = 0.001;
	static const float farDistance = 10000;
	int width;
	int height;
	float realWidth;
	float realHeight;
	float realDistance;
	const int displayList;



	virtual void Draw() const;
	virtual void Update(float deltaT);
	void DrawForEye() const;

	//RFB::UPDATERECEIVERのためのメソッド
	virtual void Setup(int w, int h, FORMAT);
	virtual void Update(int x, int y, int w, int h, void*);
};


#endif
