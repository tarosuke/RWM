#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "room.h"
#include "texture.h"
#include "linkPanel.h"
#include <avatar/avatar.h>



TOOLBOX::QUEUE<ROOM> ROOM::rooms;

ROOM::ROOM() : roomsNode(*this){
	rooms.Add(roomsNode);
}


ROOM::~ROOM(){
}


void ROOM::VertexAndTexture(unsigned c, float hl, float vl) const{
	const float up((hl / walls[c].texture.size.u) +
		walls[c].texture.offset.u);
	glTexCoord2f(up,
		   (vl / walls[c].texture.size.v) +
		   walls[c].texture.offset.v);
	glVertex3f(walls[c].x, ceilHeight, walls[c].z);
	glTexCoord2f(up, 0);
	glVertex3f(walls[c].x, floorHeight, walls[c].z);
}

void ROOM::FloorAndCeil(float h,
	const PLANE::TEXTURE& t,
	const TEXTURE& texture) const{
	glMaterialfv(GL_FRONT_AND_BACK,
		GL_AMBIENT_AND_DIFFUSE, t.color.raw);
	texture.Bind(t.id);
	glBegin(GL_POLYGON);
	for(int c(numOfWalls - 1); 0 <= c; c--){
		const float x(walls[c].x);
		const float z(walls[c].z);

		glTexCoord2f(x / t.size.u + t.offset.u,
			z / t.size.v + t.offset.v);
		glVertex3f(x, h, z);
	}
	glEnd();
}

void ROOM::Draw(unsigned remain, const TEXTURE& texture) const{
	if(!remain){
		//もう追いかけない
		return;
	}

	glEnable(GL_TEXTURE);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//部屋の明るさ
	glLightfv(GL_LIGHT0, GL_AMBIENT, brightness.raw);

	//壁や床に開ける穴(描画する代わりにステンシルバッファを-1)
	glStencilFunc(GL_ALWAYS, remain - 1, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	for(TOOLBOX::QUEUE<LINKPANEL>::ITOR i(panels); i; i++){
		(*i.Owner()).Draw(remain - 1, texture);
	}
	glStencilFunc(GL_EQUAL, remain, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	//再び描画できるようにする
	glColorMask(1,1,1,1);
	glDepthMask(1);

	//床
	glFrontFace(GL_CW);
	glNormal3f(0, 1, 0);
	FloorAndCeil(floorHeight, floor, texture);

	//天井
	glFrontFace(GL_CCW);
	glNormal3f(0, -1, 0);
	FloorAndCeil(ceilHeight, ceil, texture);

	//壁を一気に配置(ステンシルバッファ <= remainの時だけ)
	//テクスチャ設定or解除＆材質設定
	texture.Bind(walls[0].texture.id);
	glMaterialfv(GL_FRONT_AND_BACK,
		GL_AMBIENT_AND_DIFFUSE, walls[0].texture.color.raw);

	//座標設定開始
	glFrontFace(GL_CW);
	glBegin(GL_TRIANGLE_STRIP);
	float hl(0.0);
	const float vl(ceilHeight - floorHeight);
	for(int c(0); c < numOfWalls; c++){
		//法線設定
		glNormal3fv(walls[c].normal);

		//座標とテクスチャ位置設定
		VertexAndTexture(c, hl, vl);

		//水平長更新
		const int cn((c + 1) % numOfWalls);
		const float xl(walls[c].x - walls[cn].x);
		const float zl(walls[c].z - walls[cn].z);
		hl += sqrt(xl*xl + zl*zl);
	}
	//最初の座標を設定して部屋を閉じる
	VertexAndTexture(0, hl, vl);
	glEnd();

	//部屋以外の物体はステンシルテストはしない
	glDisable(GL_STENCIL_TEST);

	//TODO:固定オブジェクト

	//TODO:可動オブジェクト

	//部屋にいるAVATAR
	for(TOOLBOX::QUEUE<AVATAR>::ITOR i(avatars); i; i++){
		(*(i.Owner())).Draw();
	}

	//TODO:隣の部屋
}


const ROOM& ROOM::Move(double& x, double& y, double& dx, double& dy, float radius) const{
	//まず移動してみる
	x += dx;
	y += dy;

	//TODO:ゲートを通り抜けていたら通った先の部屋を返す

	//壁を突き破っていたら連れ戻す。
	for(int i(0); i < numOfWalls; i++){
		//壁の座標と法線をメモ
		const float wpx(walls[i].x);
		const float wpy(walls[i].z);
		const float nx(-walls[i].normal[0]); //x-zを切り取ると逆になる
		const float ny(walls[i].normal[2]);

		//radiusでオフセットしたwpx,wpyを原点にする
		const float wx(x - wpx + nx * radius);
		const float wy(y - wpy + ny * radius);

		//壁側のΔ
		const int c((i + 1) % numOfWalls);
		const float dwx(walls[c].x - wpx);
		const float dwy(walls[c].z - wpy);

		//の正規化値
		const float dwLen(sqrt(dwx*dwx + dwy*dwy));
		const float ndwx(dwx / dwLen);
		const float ndwy(dwy / dwLen);

		//dw×ndwを計算(左、正なら内側)
		const float distance(wx*ndwy - ndwx*wy);
		if(distance < 0){
			//線分範囲にあることを確認
			const float wl(wx*ndwx + wy*ndwy);
			if(0 <= wl && wl <= dwLen){
				//連れ戻して
				x += distance * nx;
				y += distance * ny;
				//跳ね返す
				dx += distance * nx * 2;
				dy += distance * ny * 2;

				//弾かれて既に確認済みの壁に当たるとチェックされないので再チェック
				continue;
			}
		}
	}
	return *this;
}


void ROOM::GetCenter(float& x, float& y, float& z) const{
	x = z = 0;
	for(int i(0); i < numOfWalls; i++){
		x += walls[i].x;
		z += walls[i].z;
	}
	x /= numOfWalls;
	z /= numOfWalls;
	y = floorHeight;
}


void ROOM::RegisterAvatar(TOOLBOX::NODE<AVATAR>& node) const{
	avatars.Add(node);
}

void ROOM::RegisterLinkPanel(TOOLBOX::NODE<LINKPANEL>& node){
	panels.Add(node);
}



ROOM::ROOMLINK::ROOMLINK(unsigned rn) : roomNo(rn), target(0){}

ROOM::ROOMLINK::operator const ROOM*(){
	if(target){
		return target;
	}

	TOOLBOX::QUEUE<ROOM>::ITOR i(ROOM::rooms);
	for(unsigned n(0); n < roomNo; n++, i++){
		if(!i){
			return 0;
		}
	}
	return i.Owner();;
}


