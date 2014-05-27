

#include <view/view.h>


class Ambient : public VIEW::DRAWER{
public:
	//描画オブジェクトの抽象クラス
	class Room;
	class Object{
	public:
		Object(TOOLBOX::QUEUE<Object>& to) : node(*this){
			to.Add(node);
		};
		virtual void Draw() const=0;
		Room* next;
	protected:
	private:
		TOOLBOX::NODE<Object> node;

	};

	//「部屋」
	class Room{
	public:
		void Draw(const unsigned level);
	protected:
		TOOLBOX::QUEUE<Object> gates;
		TOOLBOX::QUEUE<Object> borders;
		TOOLBOX::QUEUE<Object> transparents;
		unsigned sequence;
	};

	Ambient(VIEW& v);
private:
	static unsigned sequence; //描画済の部屋を識別するためのもの
	static Room* in; //今いる部屋
	void Draw() const;

	static const unsigned stencilBitMask = 15;
	static const unsigned maxLevel = 15; //15回部屋を通ったらその先は描画しない
};

