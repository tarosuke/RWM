#pragma once

#include "../gl/displayList.h"
#include "../gl/matrix.h"
#include "../toolbox/queue/queue.h"


namespace Ambient{

	class Room{
	public:
		//部屋テンプレート
		class Template{
		public:
			Template();
			~Template();
			void Draw() const { hardObjects.Playback(); };
			void Record();

			//壁床天井の衝突をチェックして位置と速度を修正する
			//リンクを通過していなければ負数、でなければリンク番号を返す
			int Collision(float& x, float& y, float& dx, float& dy);
		protected:
			virtual void DrawHardObjects()=0; //床とか天井とか描画(記録用)
		private:
			static const unsigned maxBorders = 8; //壁の最大数

			TOOLBOX::NODE<Template> node; //登録用ノード
			GL::DisplayList hardObjects; //部屋それ自体などの動かない物体
			class Border* borders[maxBorders];
		};

		void Draw(unsigned) const; //部屋の描画(formをDrawして次の部屋のDrawを呼ぶ)
	protected:
		Room();
		~Room();

	private:
		mutable TOOLBOX::NODE<Room> node; //登録用ノード
		mutable TOOLBOX::NODE<Room> linkNode; //隣の部屋用ノード
		const Template& form;
		GL::Matrix modelMatrix;
		mutable TOOLBOX::QUEUE<Room> links;
	};

}

