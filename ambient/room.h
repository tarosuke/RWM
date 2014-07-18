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
			void Draw(){ hardObjects.Playback(); };
			void Record();
		protected:
			virtual void DrawHardObjects()=0; //床とか壁とか描画(記録用)
		private:
			TOOLBOX::NODE<Template> node; //登録用ノード
			GL::DisplayList hardObjects; //部屋それ自体などの動かない物体
		};

		void Draw(unsigned); //部屋の描画(formをDrawして次の部屋のDrawを呼ぶ)
	protected:
		Room();
		~Room();

	private:
		TOOLBOX::NODE<Room> node; //登録用ノード
		TOOLBOX::NODE<Room> linkNode; //隣の部屋用ノード
		Template& form;
		GL::Matrix viewMatrix;
		TOOLBOX::QUEUE<Room> links;
	};

}

