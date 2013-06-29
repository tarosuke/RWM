#ifndef _ROOM_
#define _ROOM_

#include <assert.h>

#include <GL/gl.h>


class ROOM{
public:
	static void Load(bool test);
	static void Unload();
	static void Update(int remain){
		assert(userIn);
		(*userIn).Draw(remain);
	};
	ROOM();
	~ROOM();
	union COLOR{
		struct{
			float r;
			float g;
			float b;
			float a;
		};
		float raw[4];
	};
protected:
	static const float boxSize = 5000;
	static ROOM* userIn;
	static const char* rcPath;

	int numOfWalls;
	struct{
		//スタート位置(次の壁のスタート位置までがこの壁)
		float x;
		float z;
		//法線
		float normal[3];
		//テクスチャ
		struct{
			int id; //範囲外ならテクスチャなし(色だけ適用)
			COLOR color;
			struct{
				//テクスチャの床面、スタート位置でのオフセット[pixel]
				int u;
				int v;
			}offest;
			struct{
				//テクスチャのサイズ[m]
				float u;
				float v;
			}size;
		}texture;
	}ceil, floor, walls[6];
	float floorHeight;
	float ceilHeight;
	COLOR brightness;

	void Draw(int remain);
};




class TESTROOM : public ROOM{
public:
	TESTROOM(){
		userIn = this;

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
			walls[c].texture.id = 0;
			walls[c].texture.color.r =
			walls[c].texture.color.g =
			walls[c].texture.color.b = 0.5;
			walls[c].texture.color.a = 1;
			walls[c].normal[0] = normals[c][0];
			walls[c].normal[1] = normals[c][1];
			walls[c].normal[2] = normals[c][2];
		}

		floorHeight = 0.0;
		ceilHeight = 2.4;

		brightness.r =
		brightness.g =
		brightness.b = 0.7;
		brightness.a = 1;
	};
};


#endif
