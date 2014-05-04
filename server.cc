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
	static char text[127];
	while(be && 0 < sock.Receive(text, sizeof(text))){
		puts(text);
	}
}



namespace PACKET{
	void FEATURECHECK::Do(SOCKET& s){
		FEATUREANSWER ans(0);
		ans.SendTo(s);
	}
	void FEATUREANSWER::Do(SOCKET&){}
}

