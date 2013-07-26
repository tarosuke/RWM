#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include <world/texture.h>
#include <image/ppm.h>
#include <view/riftView.h>
#include <world/room.h>
#include <world/linkPanel.h>
#include <avatar/humanoid.h>
#include <ghost/riftGhost.h>
#include <window.h>



int main(int argc, char *argv[]){
	static WINDOW::ROOT root;
	static TESTROOM testRoom(0);
	static TESTROOM testRoom1(3);
	testRoom.RegisterLinkPanel((*new LINKPANEL(2)).roomsNode);
	static HUMANOID::PROFILE profile(1.66);
	static PPM textureImage("textureSet.ppm");
	static TEXTURE texture(textureImage);
	static GHOST& user(*new RIFTGHOST);
	static RIFTVIEW view(
		*new HUMANOID(testRoom, profile, user),
		texture);
	static HUMANOID::PROFILE buddyProfile(1.2, 1.1, 0.8);
	static HUMANOID buddy(testRoom, buddyProfile, *new GHOST);
	root.Run(user);

	return 0;
}
