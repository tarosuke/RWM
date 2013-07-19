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



int main(int argc, char *argv[]){
	static TESTROOM testRoom;
	static HUMANOID::PROFILE profile(1.66);
	static PPM textureImage("textureSet.ppm");
	static TEXTURE texture(textureImage);
	static GHOST& user(*new RIFTGHOST);
	static RIFTVIEW view(
		*new HUMANOID(testRoom, profile, user),
		texture);
	static HUMANOID::PROFILE buddyProfile(1.2, 1.1);
	static HUMANOID buddy(testRoom, buddyProfile, *new GHOST);
	WINDOW::root.Run(user);

	return 0;
}
