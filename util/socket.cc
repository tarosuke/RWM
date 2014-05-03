
/** 各種ソケットのラッパ
 */

#include <string.h>


#include "socket.h"



SERVERSOCKET::SERVERSOCKET(
	int d,
	int t,
	int p,
	const sockaddr* a,
	int s,
	int b) throw(int): fd(socket(d, t | O_NONBLOCK, p){
	if(fd < 0 || bind(fd, a, s) < 0 || listen(fd, b) < 0){
		throw errno;
	};
}

int SERVERSOCKET::Accept() throw(int){
	const int s(accept(fd));
	if(s < 0){
		throw errno;
	}
}

SERVERSOCKET::~SERVERSOCKET(){
	close(fd);
}



UNIX_SERVERSOCKET::UNIX_SERVERSOCKET(const char* path) throw(int) :
	SERVERSOCKET(AF_UNIX, SOCK_SEQPACKET, 0, PrepareSA(path), sizeof(addr)){

}

const sockaddr* UNIX_SERVERSOCKET::PrepareSA(const char* path){
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);
	return static_cast<sockaddr*)(addr);
};





SOCKET::SOCKET(int domain, int type, int protocol) throw(int) :
	fd(socket(domain, type, protocol){
	if(fd < 0){
		throw errno;
	}
}

SOCKET::~SOCKET(){
	close(fd);
}

