#ifndef _ROOM_
#define _ROOM_

#include <assert.h>

#include <GL/gl.h>

#include <toolbox/queue/queue.h>



class ROOM{
public:
	union COLOR{
		struct{
			float r;
			float g;
			float b;
			float a;
		};
		float raw[4];
	};
	ROOM();
	~ROOM();
	const ROOM& Move(double& x, double& y, double& dx, double& dy, float radial) const;
	void GetCenter(float& x, float& y, float& z) const;
	void Draw(unsigned remain, const class TEXTURE&) const;
	void RegisterAvatar(TOOLBOX::NODE<class AVATAR>& node) const;
	void RegisterLinkPanel(TOOLBOX::NODE<class LINKPANEL>&);
	const TOOLBOX::QUEUE<class AVATAR>& GetAvatars(){
		return avatars;
	};
	class ROOMLINK{
	public:
		ROOMLINK(unsigned roomNo);
		operator const ROOM*();
	private:
		unsigned roomNo;
		ROOM* target;
	};
protected:
	static TOOLBOX::QUEUE<ROOM> rooms;
	TOOLBOX::NODE<ROOM> roomsNode;
	static const float boxSize = 5000;

	mutable TOOLBOX::QUEUE<class LINKPANEL> panels;

	int numOfWalls;
	struct PLANE{
		//スタート位置(次の壁のスタート位置までがこの壁)
		float x;
		float z;
		//法線
		float normal[3];
		//テクスチャ
		struct TEXTURE{
			unsigned id; //範囲外ならテクスチャなし(色だけ適用)
			COLOR color;
			struct{
				//テクスチャの床面、スタート位置でのオフセット[pixel]
				int u;
				int v;
			}offset;
			struct{
				//テクスチャのサイズ[m]
				float u;
				float v;
			}size;
		}texture;
	}walls[6];
	PLANE::TEXTURE ceil, floor;
	float floorHeight;
	float ceilHeight;
	COLOR brightness;

	mutable TOOLBOX::QUEUE<class AVATAR> avatars;

	void VertexAndTexture(
		unsigned n, float horizLen, float vertLen) const;
	void FloorAndCeil(
		float height, const PLANE::TEXTURE&, const TEXTURE&) const;
};




class TESTROOM : public ROOM{
public:
	TESTROOM(){
		const int now(4);
		numOfWalls = now;
		static const struct{
			float x;
			float z;
		}positions[] = {
			{ -2, 3 }, { 2, 3 }, { 2, -3 }, { -2, -3 } };
		static const float normals[][3]={
			{ 0, 0, 1 }, { -1, 0, 0 },
			{ 0, 0, -1 }, { 1, 0, 0 } };

		for(int c(0); c < now; c++){
			walls[c].x = positions[c].x;
			walls[c].z = positions[c].z;
			walls[c].texture.id = 4;
			walls[c].texture.color.r =
			walls[c].texture.color.g =
			walls[c].texture.color.b = 0.5;
			walls[c].texture.color.a = 1;
			walls[c].normal[0] = normals[c][0];
			walls[c].normal[1] = normals[c][1];
			walls[c].normal[2] = normals[c][2];
			walls[c].texture.offset.u =
			walls[c].texture.offset.v = 0;
			walls[c].texture.size.u =
			walls[c].texture.size.v = 0.3;
		}

		floorHeight = 0.0;
		ceilHeight = 2.4;

		ceil.id = 2;
		floor.id = 1;
		ceil.color.r =
		ceil.color.g =
		ceil.color.b =
		floor.color.r =
		floor.color.g =
		floor.color.b = 0.5;
		ceil.color.a =
		floor.color.a = 1;
		ceil.offset.u =
		ceil.offset.v =
		floor.offset.u =
		floor.offset.v = 0;
		ceil.size.u =
		ceil.size.v = 0.9;
		floor.size.u =
		floor.size.v = 0.3;

		brightness.r =
		brightness.g =
		brightness.b = 0.5;
		brightness.a = 1;
	};
};


#endif
