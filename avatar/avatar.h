/*************************************************** basic avatar class:AVATAR
 * avatarと言っても視点しかなく、体はHUMANOIDのように導出、拡張して付け足す。
 * また、HUMANOIDと言っても姿がない基底クラスなのでこれも導出して姿をレンダリングする。
 * 体の姿勢他は視点の移動に追随して(IKとかで)生成される。
 */
#ifndef _AVATAR_
#define _AVATAR_

#include <toolbox/qon//qon.h>
#include <toolbox/queue/queue.h>


class AVATAR{
	AVATAR();
	void operator=(AVATAR&);
public:
	//例えばジョイスティックのボタンで発動するようなアクション
	class ACTION{
	};
	//システムハンドラ
	virtual void GetView() const;
	virtual void Draw() const = 0;
	virtual void Update(float deltaT);
	//構築/破壊子
	AVATAR(const class WORLD& initialWorld,
		class GHOST&,
		float y);
	virtual ~AVATAR();
	//情報取得
	const class ROOM& GetRoomIn(){ return *in; };
protected:
	const class ROOM* in;
	TOOLBOX::NODE<AVATAR> roomNode;
	class GHOST& ghost;
	VQON position;
	VQON velocity;
	QON direction;
	double rotVelocity;
	void GetModel() const;
};




#endif
