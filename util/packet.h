#pragma once

#include "socket.h"



namespace PACKET{


	class PACKET{
	public:
		bool SendTo(SOCKET&);
		static void Receive(SOCKET&); //PACKETのファクトリでもあるが受信PACKETは自己消滅なので外部にインスタンスが見えることはない
	protected:
		PACKET(unsigned short type, unsigned short size, void* buff);
	private:
		static unsigned short seqence;
		virtual void Do()=0; //パケット処理
		virtual ~PACKET();
		unsigned short header[4];
		void* const body;
		const short length;
	};












	class FEATURECHECK : public PACKET{
	};

}
