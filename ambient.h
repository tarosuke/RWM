

#include <view/view.h>


class Ambient : public VIEW::DRAWER{
public:
	//ジオメトリのための構造体
	struct V2{
		float x;
		float z;
	};

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
		Room(const float* m=0);
		void Draw(const unsigned level);
	protected:
		TOOLBOX::QUEUE<Object> gates;
		TOOLBOX::QUEUE<Object> borders;
		TOOLBOX::QUEUE<Object> transparents;
		unsigned sequence;
	private:
		TOOLBOX::NODE<Room> node;
		float viewMatrix[16]; //view変換行列
	};

	Ambient(VIEW& v);
private:
	static unsigned sequence; //描画済の部屋を識別するためのもの
	static Room* in; //今いる部屋
	static TOOLBOX::QUEUE<Room> rooms; //すべての部屋
	void Draw() const;

	static const unsigned stencilBitMask = 15;
	static const unsigned maxLevel = 15; //15回部屋を通ったらその先は描画しない
};



class SquareRoom : public Ambient::Room{
public:
	SquareRoom(float width, float depth, float height = 2.4);
	class RoundWall : public Ambient::Object{
	public:
		RoundWall(TOOLBOX::QUEUE<Ambient::Object>& to, float w, float d, float h);
		void Draw() const;
	private:
		float vertexes[5][2][3];
	}roundWall;
};
