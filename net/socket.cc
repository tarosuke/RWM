
/** 各種ソケットのラッパ
 */

#include <string.h>
#include <unistd.h>
#include <stdio.h>


#include "socket.h"

namespace NET{

	SERVERSOCKET::SERVERSOCKET(
		int d,
		int t,
		int p,
		const sockaddr* a,
		int s,
		int b) throw(int): fd(socket(d, t, p)){
		if(fd < 0 || bind(fd, a, s) < 0 || listen(fd, b) < 0){
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
		close(fd);
	}



	UNIX_SERVERSOCKET::UNIX_SERVERSOCKET(const char* path, int type) throw(int) :
		SERVERSOCKET(AF_UNIX, type, 0, PrepareSA(path), sizeof(addr)){
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

	void SOCKET::Send(const char* str){
		Send(str, strlen(str));
	}

	SOCKET::~SOCKET(){
		close(fd);
	}



	UNIX_SOCKET::UNIX_SOCKET(const char* path, int type) throw(int) :
		SOCKET(AF_UNIX, type, 0){
		sockaddr_un addr;
		addr.sun_family = AF_UNIX;
		strncpy(&addr.sun_path[1], path, sizeof(addr.sun_path) - 1);
		addr.sun_path[0] = 0;
		if(connect(fd, (sockaddr*)&addr, sizeof(addr)) < 0){
			throw errno;
		};
	}

}
