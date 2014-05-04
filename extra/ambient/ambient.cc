#include <stdio.h>
#include <unistd.h>

#include <socket.h>
#include <packet.h>

#include "client.h"


int main(){
	try{
		static UNIX_SOCKET sock("rwm");
		PACKET::FEATURECHECK featureCheck;
		featureCheck.SendTo(sock);

		for(;;sleep(10));
	}
	catch(int){
		puts("Failed to connect RWM.");
	}
}
