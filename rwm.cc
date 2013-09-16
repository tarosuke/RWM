#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include <view/riftView.h>
#include <rfb/rfb.h>



int main(int argc, char *argv[]){
	static VIEW& view(*new RIFTVIEW());
	static RFB rfb(view);
	return 0;
}
