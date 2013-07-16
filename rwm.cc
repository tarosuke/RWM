#include <GL/glut.h>

#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include <room/texture.h>
#include <image/ppm.h>
#include <riftView.h>
#include <room/room.h>
#include <avatar/humanoid.h>
#include <riftghost.h>


static VIEW* view;
static AVATAR* avatar;


static void display(void){
	if(view){
		(*view).Draw();
		glutSwapBuffers();
	}
}

static void Idle(){
	(*view).Update(0.01); //タイムスタンプを求めてΔtを計算しとく
	glutPostRedisplay();
	glutForceJoystickFunc();
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

int main(int argc, char *argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(1280, 800);
	glutCreateWindow(argv[0]);

	static TESTROOM testRoom;
	static HUMANOID::PROFILE profile(1.66);
	avatar = new HUMANOID(testRoom, profile, *new RIFTGHOST);
	static PPM textureImage("textureSet.ppm");
	static TEXTURE texture(textureImage);
	static RIFTVIEW viewBody(*avatar, texture);
	view = &viewBody;

	glutDisplayFunc(display);
	glutJoystickFunc(Joystick, 20);
	glutIdleFunc(Idle);
	glutKeyboardFunc(Key);
	glutSpecialFunc(SKey);
	glutMainLoop();
	return 0;
}
