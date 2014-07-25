/*****************************************************************************
 * 壁、天井、床なんかの抽象クラス
 */

#pragma once


namespace Ambient{

	class Border{
	public:
		Border();
		virtual void Draw()=0;
		virtual bool Collision(float& x, float& y, float& dx, float& dy)=0;
	};

}
