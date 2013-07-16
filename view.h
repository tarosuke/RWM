#ifndef _VIEW_
#define _VIEW_

class VIEW{
public:
	VIEW(int width, int height, class AVATAR&, const class TEXTURE&);
	virtual ~VIEW();
	virtual void Draw();
	virtual void Update(float deltaT);
protected:
	static const float defaultDotPitch = 0.003;
	static const float defaultDisplayDistance = 0.7;
	static const float nearDistance = 0.1;
	static const float farDistance = 10000;
	int width;
	int height;
	float realWidth;
	float realHeight;
	float realDistance;
	const class TEXTURE& texture;
	class AVATAR& avatar;
	const int displayList;
	void DrawForEye();
};


#endif
