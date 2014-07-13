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
			void Draw(){ hardObjects.Playback(); };
		protected:
			virtual void DrawHardObjects()=0; //床とか壁とか描画(記録用)
		private:
			GL::DisplayList hardObjects; //部屋それ自体などの動かない物体
		};

		void Draw(); //部屋の描画(hardObjectsはPlaybackする)
	protected:
		Room();
		~Room();

	private:
		TOOLBOX::NODE<Room> node;
		Template& form;
		GL::Matrix viewMatrix;
	};

}

