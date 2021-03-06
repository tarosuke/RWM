#include <GL/glew.h>
#include <GL/gl.h>
#include <stdio.h>

#include "room.h"
#include "border.h"



namespace Ambient{

	void Room::Draw(unsigned remain) const{
		//view行列でアフィン変換した上でテンプレートを描画
		{
			GL::Matrix::Zone zone(modelMatrix);
			form.Draw();
		}

		//リンク先の部屋も描画
		--remain;
		for(TOOLBOX::QUEUE<Room>::ITOR i(links); i; i++){
			(*i).Draw(remain);
		}

		//TODO:部屋の中の(動く)物体を描画
	}





	Room::Template::Template() : node(*this){}
	Room::Template::~Template(){}
	void Room::Template::Record(){
		//ディスプレイリストへこの部屋テンプレートの描画内容を記録
		GL::DisplayList::Recorder r(hardObjects);
		DrawHardObjects();
	}

	int Room::Template::Collision(float& x, float& y, float& dx, float& dy){
		for(unsigned n(0); n < maxBorders; ++n){
			if(borders[n]){
				if((*borders[n]).Collision(x, y, dx, dy)){
					//リンク通過
					return n;
				}
			}
		}
		return -1;
	}
}

#if 0

Ambient::Room::Room(const float* m) : node(*this){
	Ambient::rooms.Add(node);
	if(m){
		for(unsigned n(0); n < 16; ++n){
			viewMatrix[n] = m[n];
		}
	}
};


void Ambient::Room::Draw(const TexSet& texSet, const unsigned level){
	if(sequence == Ambient::sequence){
		//描画済
		return;
	}
	sequence = Ambient::sequence;

	//ゲート描画
	glStencilFunc(GL_EQUAL, level, stencilBitMask);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glColorMask(0,0,0,0);
	glDepthMask(0);
	gates.Each(&Object::Draw, texSet);
	glColorMask(1,1,1,1);
	glDepthMask(1);

	//壁、床、天井描画
	glStencilFunc(GL_LESS, level, stencilBitMask); //その部屋のゲートには描画しない
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	borders.Each(&Object::Draw, texSet);

	//ゲート追跡
	if(level < maxLevel){
		for(TOOLBOX::QUEUE<Object>::ITOR i(gates); i; i++){
			Room* next((*i).next);
			if(next){
				(*next).Draw(texSet, level - 1);
			}
		}
	}

	//戻りで透過物体を描画(透過物体はゲートの上に描画するのでステンシルテストはなし)
	glDisable(GL_STENCIL_TEST);
	transparents.Each(&Object::Draw, texSet);
}



SquareRoom::RoundWall::RoundWall(TOOLBOX::QUEUE<Ambient::Object>& to, const SquareRoom& room) : Ambient::Object(to), room(room){
}


void SquareRoom::RoundWall::Draw(const Ambient::TexSet& texSet) const{
	texSet.Activate(0);

	glColor4f(1.0, 1.0, 1.0, 1);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(VERTEX), &room.vertexes[0][0]);
	glTexCoordPointer(2, GL_FLOAT, sizeof(VERTEX), &(room.vertexes[0][0].u));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 10);
}


const float SquareRoom::Plane::colors[2][4] = {
	{ 1, 1, 1, 1 }, { 0.5, 0.25, 0.25, 1 } };

SquareRoom::Plane::Plane(TOOLBOX::QUEUE<Ambient::Object>& to, const SquareRoom& room, bool floor) : Ambient::Object(to), room(room), offset(floor ? 0 : 1), color(colors[floor ? 0 : 1]){}

void SquareRoom::Plane::Draw(const Ambient::TexSet& texSet) const{
	texSet.Activate(0);

	glColor4fv(color);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(VERTEX) * 2, &room.vertexes[0][offset]);
	glTexCoordPointer(2, GL_FLOAT, sizeof(VERTEX) * 2, &(room.vertexes[0][offset].u));
	glDrawArrays(GL_TRIANGLE_FAN, 0, 10);
}


SquareRoom::SquareRoom(float width, float depth, float height) : roundWall(borders, *this), floor(borders, *this, true), ceil(borders, *this, false){
	for(unsigned n(0); n < 5; ++n){
		const float x(n & 2 ? width/2 : -width/2);
		const float z((n + 1) & 2 ? depth/2 : -depth/2);

		//頂点座標
		vertexes[n][0].x =
		vertexes[n][1].x = x;
		vertexes[n][0].y = height - 1.6;
		vertexes[n][1].y = -1.6;
		vertexes[n][0].z =
		vertexes[n][1].z = z;

		//u/v座標
		vertexes[n][0].u =
		vertexes[n][1].u = x;
		vertexes[n][0].v =
		vertexes[n][1].v = z * 1.732;
	}
}


#endif
