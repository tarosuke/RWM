///// パケット共通コード


#include "../util/packet.h"

#include <stdio.h>


namespace PACKET{

	unsigned short PACKET::sequence(0);

	PACKET::PACKET(TYPE type, unsigned short size, void* buff) :
		body(buff),
		length(size){
		header.type = (unsigned short)type;
		header.length = size;
		header.sequence = ++sequence;
		header._reserved = 0;
	}

	bool PACKET::SendTo(SOCKET& s){
		//ヘッダ送信
		s.Send(&header, sizeof(header));
		//ボディ
		if(length && body){
			s.Send(body, length);
		}
		return true;
	}
}

