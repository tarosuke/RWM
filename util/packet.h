#pragma once

#include "socket.h"



namespace PACKET{


	class PACKET{
	public:
		enum TYPE{
			FeatureCheck,
			FeatureAnswer,
		};
		bool SendTo(SOCKET&);
		static void Receive(SOCKET&); //受信パケットに応じたPACKETのファクトリでもあるが受信PACKETは自己消滅なので外部にインスタンスが見えることはない
	protected:
		PACKET(TYPE type, unsigned short size, void* buff);
		virtual ~PACKET(){};
	private:
		static unsigned short sequence;
		virtual void Do(SOCKET&)=0; //パケット処理
		unsigned short header[4];
		void* const body;
		const short length;
	};












	class FEATURECHECK : public PACKET{
	public:
		FEATURECHECK() : PACKET(PACKET::FeatureCheck, 0, 0){};
	private:
		void Do(SOCKET&);
	};

	class FEATUREANSWER : public PACKET{
	public:
		FEATUREANSWER(unsigned ans) :
			PACKET(PACKET::FeatureAnswer,
			       sizeof(unsigned),
			       &(*this).features), features(ans){};
	private:
		unsigned features;
		void Do(SOCKET&);
	};

}
