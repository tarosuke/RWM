#ifndef _VIEW_
#define _VIEW_

#include <toolbox/queue/queue.h>


class VIEW{
public:
	static const unsigned roomFollowDepth = 16;
	VIEW(int width, int height, class AVATAR&);
	virtual ~VIEW();
	static void DrawAll(){
		for(TOOLBOX::QUEUE<VIEW>::ITOR i(views); i; i++){
			VIEW* v(i.Owner());
			if(v){
				(*v).Draw();
			}
		}
	};
	static void UpdateAll(float deltaT){
		for(TOOLBOX::QUEUE<VIEW>::ITOR i(views); i; i++){
			VIEW* v(i.Owner());
			if(v){
				(*v).Update(deltaT);
			}
		}
	};
protected:
	static const float defaultDotPitch = 0.003;
	static const float defaultDisplayDistance = 0.7;
	static const float nearDistance = 0.1;
	static const float farDistance = 10000;
	static TOOLBOX::QUEUE<VIEW> views;
	TOOLBOX::NODE<VIEW> node;
	int width;
	int height;
	float realWidth;
	float realHeight;
	float realDistance;
	class AVATAR& avatar;
	const int displayList;
	virtual void Draw() const;
	virtual void Update(float deltaT);
	void DrawForEye() const;
};


#endif
