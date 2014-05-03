
/** 各種ソケットのラッパ
 */

#include <string.h>
#include <unistd.h>
#include <stdio.h>


#include "socket.h"



SERVERSOCKET::SERVERSOCKET(
	int d,
	int t,
	int p,
	const sockaddr* a,
	int s,
	int b) throw(int): fd(socket(d, t, p)){
	if(fd < 0 || bind(fd, a, s) < 0 || listen(fd, b) < 0){
		printf("listen failed:%d.\n", b);
		throw errno;
	};
}

int SERVERSOCKET::Accept() throw(int){
	const int s(accept(fd, 0, 0));
	if(s < 0){
		throw errno;
	}
	return s;
}

SERVERSOCKET::~SERVERSOCKET(){
	puts("close");
	close(fd);
}



UNIX_SERVERSOCKET::UNIX_SERVERSOCKET(const char* path) throw(int) :
	SERVERSOCKET(AF_UNIX, SOCK_STREAM, 0, PrepareSA(path), sizeof(addr)){
}

const sockaddr* UNIX_SERVERSOCKET::PrepareSA(const char* path){
	addr.sun_family = AF_UNIX;
	strncpy(&addr.sun_path[1], path, sizeof(addr.sun_path) - 1);
	addr.sun_path[0] = 0;
	return (sockaddr*)&addr;
};





SOCKET::SOCKET(int domain, int type, int protocol) throw(int) :
	fd(socket(domain, type, protocol)){
	if(fd < 0){
		throw errno;
	}
}

SOCKET::~SOCKET(){
	close(fd);
}



UNIX_SOCKET::UNIX_SOCKET(const char* path) : SOCKET(AF_UNIX, SOCK_STREAM, 0){
	sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	strncpy(&addr.sun_path[1], path, sizeof(addr.sun_path) - 1);
	addr.sun_path[0] = 0;
	connect(fd, (sockaddr*)&addr, sizeof(addr));
}

