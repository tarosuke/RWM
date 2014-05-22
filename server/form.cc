
#include <stdlib.h>

#include <GL/glew.h>

#include "form.h"


const FORM* FORM::forms(0); //形状情報の配列(読み込み完了後に有効)
TOOLBOX::QUEUE<FORM> FORM::stocks;
TOOLBOX::QUEUE<FORM> FORM::drawTargets;


///// 描画関連

void FORM::DrawAll(){
	//VBOでの描画準備
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	for(TOOLBOX::QUEUE<FORM>::ITOR i(drawTargets); i; i++){
		(*i).Draw();
	}

	//後始末
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

void FORM::DrawTransparents(){}





void FORM::Draw(){
	//下準備
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	const GLsizei stride(sizeof(VERTEX));
	const VERTEX& o(*(VERTEX*)0);
	glVertexPointer(3, GL_FLOAT, stride, (void*)&o.vertex);
	glNormalPointer(GL_FLOAT, stride, (void*)&o.normal);
	glColorPointer(4, GL_FLOAT, stride, (void*)&o.color);
	glTexCoordPointer(2, GL_FLOAT, stride, (void*)&o.texCoord);

	glMultiDrawArrays(drawMode, startIndex, counts, numOfPrimitives);
}




///// パケット関連

FORM::FORM(SOCKET& s, const HEADER& h) :
	PACKET(s, h),
	stockNode(*this),
	drawNode(*this){}

FORM::~FORM(){
	if(vertexBufferID){ glDeleteBuffers(1, &vertexBufferID); }
	if(startIndex){ delete startIndex; }
	if(counts){ delete counts; }
}


bool FORM::Do(SOCKET& s){
	//データの解析
	const ROOT& form(*static_cast<ROOT*>(GetBody()));
	drawMode = form.type;
	numOfPrimitives = form.numOfPrimitives;
	const unsigned numOfVertexes(form.numOfVertexes);
	VERTEX* const vertexes((VERTEX*)&form.primitives[numOfPrimitives + 1]);

	//プリミティブ情報の取得

	//頂点データのVBOへの格納
	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(VERTEX) * numOfVertexes,
		vertexes,
		GL_STATIC_DRAW);

	return false; //自分でメモリ管理する
}


