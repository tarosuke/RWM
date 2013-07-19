#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include <room/texture.h>
#include <image/ppm.h>
#include <riftView.h>
#include <room/room.h>
#include <avatar/humanoid.h>
#include <ghost/riftGhost.h>
#include <window.h>



#if 0
static VIEW* view;
static AVATAR* avatar;


static void display(void){
	WINDOW::root.Draw(&view);
}

static void Idle(){
	(*view).Update(0.01); //TODO:タイムスタンプを求めてΔtを計算しとく
}

static void Key(unsigned char key, int x, int y){
	if(!avatar){
		return;
	}
	switch(key){
	case ' ' :
		(*avatar).Actions(1);
		break;
	case ';' :
		(*avatar).SideStep(-0.1);
		break;
	case 'q' :
		(*avatar).SideStep(0.1);
		break;
	}
}

static void SKey(int key, int x, int y){
	if(!avatar){
		return;
	}
	switch(key){
	case GLUT_KEY_LEFT :
		(*avatar).Rotate(-0.03);
		break;
	case GLUT_KEY_RIGHT :
		(*avatar).Rotate(0.03);
		break;
	case GLUT_KEY_UP :
		(*avatar).ForwardStep(0.02);
		break;
	case GLUT_KEY_DOWN :
		(*avatar).ForwardStep(-0.01);
		break;
	default:
		break;
	}
}

static void Joystick(unsigned button, int x, int y, int z){
// printf("%4d %4d %4d %8x.\r", x, y, z, button);
	(*avatar).Step(0.01 * y * 0.001, 0.01 * x * 0.001);
}
#endif

int main(int argc, char *argv[]){
	static TESTROOM testRoom;
	static HUMANOID::PROFILE profile(1.66);
	static PPM textureImage("textureSet.ppm");
	static TEXTURE texture(textureImage);
	static GHOST& user(*new RIFTGHOST);
	static RIFTVIEW view(
		*new HUMANOID(testRoom, profile, user),
		texture);
// 	view = &viewBody;
	WINDOW::root.Run(user);

	return 0;
}
