#include <server.h>
#include <stdio.h>
#include <unistd.h>
#include <util/packet.h>


VIEW* SERVER::view(0);
pthread_t SERVER::listenerThread;
bool SERVER::be(true);

bool SERVER::Start(VIEW& v, const char* p){
	try{
		static UNIX_SERVERSOCKET server(p);
		view = &v;
		pthread_create(&listenerThread, 0, SERVER::Listener, (void*)&server);
		return true;
	}
	catch(int e){
		return false;
	}
}

void* SERVER::Listener(void* s){
	SERVERSOCKET& server(*(SERVERSOCKET*)s);
	try{
		while(be){
			new SERVER(server); //接続待ちでブロック
		}
	}
	catch(int){}
	pthread_exit(0);
	return 0;
}



SERVER::SERVER(SERVERSOCKET& server) : sock(server){
	pthread_create(&serverThread, 0, SERVER::_ServerThread, (void*)this);
}

void* SERVER::_ServerThread(void* t){
	SERVER* const s((SERVER*)t);
	(*s).ServerThread();
	delete s;
	pthread_exit(0);
	return 0;
}

void SERVER::ServerThread(){
#if 0
	static char c;
	while(be && 0 < sock.Receive(&c, 1)){
		printf("%02x\n", c);
	}
#else
	try{
		while(be){
			PACKET::PACKET::Receive(sock);
		}
	}
	catch(int){
		puts("disconnected");
	}
#endif
}



namespace PACKET{
// 	const char* IDENT::myName = "RWM";
// 	IDENT() : PACKET(PACKET::Ident, strlen(myName), myName){};
// 	IDENT(HEADER& h) : PACKET(PACKET::Ident, h.length, malloc(h.length)){}


	bool FEATURECHECK::Do(SOCKET& s){
		puts("FEATURECHECK::Do");
		FEATUREANSWER ans(0);
		ans.SendTo(s);
		return true;
	}
	bool FEATUREANSWER::Do(SOCKET&){ return true; }
}

