#include <stdio.h>
#include <unistd.h>

#include <socket.h>


int main(){
	puts("HELLO AMBIENT.");

	UNIX_SOCKET("rwm");
	puts("connected");
	for(;;sleep(10));
}
