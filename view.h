#ifndef _VIEW_
#define _VIEW_

class VIEW{
public:
	VIEW(int width, int height/*, const class TEXTURE&*/);
	void Draw();
protected:
	int width;
	int height;
// 	const class TEXTURE& texture;
};


#endif
