#include "packet.h"


namespace PACKET{

	unsigned short PACKET::sequence(0);

	PACKET::PACKET(unsigned short type, unsigned short size, void* buff) :
		body(buff),
		length(size){
		header[0] = type;
		header[1] = size;
		header[2] = ++seqence;
		header[3] = 0;
	}

	bool PACKET::SendTo(SOCKET& s){
		//ヘッダ送信
		s.Send(header, sizeof(header));
		//ボディ
		s.Send(body, length);
	}
}

