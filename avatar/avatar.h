/*************************************************** basic avatar class:AVATAR
 * avatarと言っても視点しかなく、体はHUMANOIDのように導出、拡張して付け足す。
 * また、HUMANOIDと言っても姿がない基底クラスなのでこれも導出して姿をレンダリングする。
 * 体の姿勢他は視点の移動に追随して(IKとかで)生成される。
 */
#ifndef _AVATAR_
#define _AVATAR_

#include <toolbox/qon//qon.h>
#include <toolbox/queue/queue.h>

#include <world/object.h>


class AVATAR : public FALLABLE{
	AVATAR();
	void operator=(AVATAR&);
public:
	//例えばジョイスティックのボタンで発動するようなアクション
	class ACTION{
	};
	//システムハンドラ
	virtual void GetView() const;
	//QBJECTハンドラ
	void Draw(const class TEXSET&) const;
	void Update(float deltaT);
	//構築/破壊子
	AVATAR(const class WORLD& initialWorld,
		class GHOST&,
		float y);
	virtual ~AVATAR();
protected:
	class GHOST& ghost;
	QON direction;
	double rotVelocity;
	void GetModel() const;
	virtual void DrawBody(const class TEXSET&) const{};
	virtual void DrawHead(const class TEXSET&) const{};
private:
};




#endif
