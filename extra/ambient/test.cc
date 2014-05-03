#include <stdio.h>
#include <unistd.h>

#include <socket.h>


int main(){
	puts("HELLO AMBIENT.");

	UNIX_SOCKET("rwm");
	for(;;sleep(10));
}
