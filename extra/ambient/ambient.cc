#include <stdio.h>
#include <unistd.h>

#include <socket.h>
#include <packet.h>

#include "client.h"


int main(){
	try{
		static UNIX_SOCKET sock("rwm/ambient");
		PACKET::FEATURECHECK featureCheck;
		featureCheck.SendTo(sock);

		try{
			for(;;){
				PACKET::PACKET::Receive(sock);
			}
		}
		catch(int){
			puts("disconnected");
			return 0;
		}
	}
	catch(int){
		puts("Failed to connect RWM.");
	}
}
