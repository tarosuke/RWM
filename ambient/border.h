/*****************************************************************************
 * 壁
 * 設定された通りの壁を描画したり、衝突判定したりする
 */

#pragma once


namespace Ambient{

	class Border{
	public:
		Border(float x0, float y0, float x1, float y1, const float hights[]);
		void Draw(); //設定された通りに壁を描画

		bool Collision(float& x, float& y, float& dx, float& dy);
	};

}
