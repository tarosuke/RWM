#include <GL/glew.h>
#include <GL/gl.h>
#include <stdio.h>

#include <ambient.h>



Ambient::Room::Room(const float* m) : node(*this){
	Ambient::rooms.Add(node);
	if(m){
		for(unsigned n(0); n < 16; ++n){
			viewMatrix[n] = m[n];
		}
	}
};


void Ambient::Room::Draw(const unsigned level){
	//既に描画されていたら戻る
	if(sequence == Ambient::sequence){
		return;
	}
	sequence = Ambient::sequence;

	//ゲート描画
	glStencilFunc(GL_EQUAL, level, stencilBitMask);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glColorMask(0,0,0,0);
	glDepthMask(0);
	gates.Each(&Object::Draw);
	glColorMask(1,1,1,1);
	glDepthMask(1);

	//壁、床、天井描画
	glStencilFunc(GL_LESS, level, stencilBitMask); //その部屋のゲートには描画しない
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	borders.Each(&Object::Draw);

	//ゲート追跡
	if(level < maxLevel){
		for(TOOLBOX::QUEUE<Object>::ITOR i(gates); i; i++){
			Room* next((*i).next);
			if(next){
				(*next).Draw(level - 1);
			}
		}
	}

	//戻りで透過物体を描画(透過物体はゲートの上に描画するのでステンシルテストはなし)
	glDisable(GL_STENCIL_TEST);
	transparents.Each(&Object::Draw);
}



SquareRoom::RoundWall::RoundWall(TOOLBOX::QUEUE<Ambient::Object>& to, float w, float d, float h) : Ambient::Object(to){
	for(unsigned n(0); n < 5; ++n){
		vertexes[n][0][0] =
		vertexes[n][1][0] = n & 2 ? w/2 : -w/2;
		vertexes[n][0][1] = h - 1.6;
		vertexes[n][1][1] = -1.6;
		vertexes[n][0][2] =
		vertexes[n][1][2] = (n + 1) & 2 ? d/2 : -d/2;
	}
}


void SquareRoom::RoundWall::Draw() const{
	glColor4f(0.7, 0.7, 1.0, 1);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, &vertexes[0][0][0]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 10);
}



SquareRoom::SquareRoom(float width, float depth, float height) : roundWall(borders, width, depth, height){}


