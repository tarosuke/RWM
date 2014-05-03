/** 各種ソケットのラッパ
 */
#pragma once


#include <sys/types.h>
#include <sys/socket.h>



/** 抽象サーバソケット
 * 接続を待つ(listenまで)
 * SOCKETのコンストラクタ引数になって初めて接続を受け付ける(acceptする)
 */
class SERVERSOCKET{
	friend class SOCKET;
	SERVERSOCKET();
	SERVERSOCKET(SERVERSOCKET&);
	void operator=(SERVERSOCKET&);
protected:
	SERVERSOCKET(
		int domain,
		int type,
		int protocol,
		const sockaddr*,
		int saSize,
		int backLogs = 16) throw(int);
	virtual ~SERVERSOCKET();
private:
	const int fd;
	int Accept() throw(int);
};


/** UNIXドメインサーバソケット
 */
class UNIX_SERVERSOCKET : public SERVERSOCKET{
	UNIX_SERVERSOCKET();
	UNIX_SERVERSOCKET(UNIX_SERVERSOCKET&);
	void operator=(UNIX_SERVERSOCKET&);
public:
	UNIX_SERVERSOCKET(const char* path) throw(int);
private:
	sockaddr_un addr;
	const sockaddr* PrepareSA(const char* path);
}



/** 一般ソケット
 * クライアントソケットとしては抽象ソケットだが、
 * サーバソケットから作った場合はそのまま使える。
 */
class SOCKET{
	SOCKET();
	void operator=(SOCKET&);
public:
	SOCKET(SERVERSOCKET& server) throw(int) : fd(server.Accept()){};
	int Send(const void* content, unsigned size, int flag=0){
		return send(fd, content, size, flag);

	};
	int Receive(void* content, unsigned size, int flag=0){
		return recv(fd, content, size, flag);
	};
	virtual ~SOCKET();
protected:
	SOCKET(int domain, int type, int protocol) throw(int);
	const int fd;
};

