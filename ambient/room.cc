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


void Ambient::Room::Draw(const TexSet& texSet, const unsigned level){
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



SquareRoom::RoundWall::RoundWall(TOOLBOX::QUEUE<Ambient::Object>& to, float w, float d, float h) : Ambient::Object(to){
	for(unsigned n(0); n < 5; ++n){
		const float x(n & 2 ? w/2 : -w/2);
		const float z((n + 1) & 2 ? d/2 : -d/2);

		//頂点座標
		vertexes[n][0].x =
		vertexes[n][1].x = x;
		vertexes[n][0].y = h - 1.6;
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


void SquareRoom::RoundWall::Draw(const Ambient::TexSet& texSet) const{
	texSet.Activate(0);

	glColor4f(1.0, 1.0, 1.0, 1);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(VERTEX), &vertexes[0][0]);
	glTexCoordPointer(2, GL_FLOAT, sizeof(VERTEX), &(vertexes[0][0].u));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 10);
}



SquareRoom::SquareRoom(float width, float depth, float height) : roundWall(borders, width, depth, height){}


