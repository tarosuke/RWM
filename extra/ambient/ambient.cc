#include <stdio.h>
#include <unistd.h>

#include <socket.h>
#include <packet.h>

#include "client.h"


int main(){
	try{
		static UNIX_SOCKET sock("rwm");
#if 0
		sock.Send("HELLO AMBIENT.");
#else
		PACKET::FEATURECHECK featureCheck;
		featureCheck.SendTo(sock);
#endif

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
