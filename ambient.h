

#include <view/view.h>
#include <raster.h>


class Ambient : public VIEW::DRAWER{
public:
	//テクスチャセットのラッパ
	class TexSet{
	public:
		TexSet(const Raster&);
		~TexSet();
		void Activate(unsigned) const;
		void Deactivate() const;
	private:
		const unsigned size;
		const unsigned numOf;
		unsigned* const ids;
	};


	//ディスプレイリストのラッパ
	class DisplayList{
	public:
		//記録用RAIIキー(存在している間は記録)
		class Recorder{
		public:
			Recorder(DisplayList& dl) : recorder(dl){ dl.StartRecord(); };
			~Recorder(){ recorder.EndRecord(); };
		private:
			DisplayList& recorder;
		};

		DisplayList();
		~DisplayList();
		void StartRecord();
		void EndRecord();
		void Playback();
	private:
		static bool recording;
		const unsigned id;
	};

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
		virtual void Draw(const TexSet&) const=0;
		Room* next;
	protected:
	private:
		TOOLBOX::NODE<Object> node;

	};

	//「壁」
	class Wall : public Object{
	public:
		struct WallDef{
			V2 points[2];
			unsigned texID;
			V2 uvPoints[2];
			Room* gate;
		};
	private:
		unsigned texID;
	};

	//「部屋」
	class Room{
	public:
		Room(const float* m=0);
		void Draw(const TexSet&, const unsigned level);
	protected:
		TOOLBOX::QUEUE<Object> gates;
		TOOLBOX::QUEUE<Object> borders;
		TOOLBOX::QUEUE<Object> transparents;
		unsigned sequence;
	private:
		TOOLBOX::NODE<Room> node;
		struct Plane{
			float height;
			unsigned texID;
			V2 uvOffset;
			V2 uvScale;
		}floor, ceil;
		float brightness;
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

	TexSet texSet;
};



class SquareRoom : public Ambient::Room{
public:
	struct VERTEX{
		float x;
		float y;
		float z;
		float u;
		float v;
	}__attribute__((packed));
	SquareRoom(float width, float depth, float height = 2.4);
	class RoundWall : public Ambient::Object{
	public:
		RoundWall(TOOLBOX::QUEUE<Ambient::Object>& to, const SquareRoom&);
		void Draw(const Ambient::TexSet&) const;
	private:
		const SquareRoom& room;
	}roundWall;
	class Plane : public Ambient::Object{
	public:
		Plane(TOOLBOX::QUEUE<Ambient::Object>&, const SquareRoom&, bool);
		void Draw(const Ambient::TexSet&) const;
	private:
		const SquareRoom& room;
		const unsigned offset;
		const float* const color;
		static const float colors[2][4];
	}floor, ceil;
private:
	VERTEX vertexes[5][2];
};
