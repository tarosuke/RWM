#include "packet.h"


namespace PACKET{

	unsigned short PACKET::sequence(0);

	PACKET::PACKET(TYPE type, unsigned short size, void* buff) :
		body(buff),
		length(size){
		header[0] = (unsigned short)type;
		header[1] = size;
		header[2] = ++sequence;
		header[3] = 0;
	}

	bool PACKET::SendTo(SOCKET& s){
		//ヘッダ送信
		s.Send(header, sizeof(header));
		//ボディ
		if(length && body){
			s.Send(body, length);
		}
		return true;
	}
}

