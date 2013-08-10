#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include <world/world.h>

#include <image/ppm.h>
#include <view/riftView.h>
#include <avatar/humanoid.h>
#include <ghost/riftGhost.h>
#include <window.h>



int main(int argc, char *argv[]){
	WINDOW::Init();
	static WORLD world(0);

	static HUMANOID::PROFILE profile(1.66);
	static GHOST& user(*new RIFTGHOST);
	static RIFTVIEW view(
		*new HUMANOID(world, profile, user));
// 	static HUMANOID::PROFILE buddyProfile(1.2, 1.1, 0.8);
// 	static HUMANOID buddy(testRoom, buddyProfile, *new GHOST);
	WINDOW::Run(user);

	return 0;
}
