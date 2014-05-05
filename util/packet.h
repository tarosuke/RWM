#pragma once

#include "socket.h"



namespace PACKET{


	class PACKET{
	public:
		enum TYPE{
			Ident,
			FeatureCheck,
			FeatureAnswer,
		};
		bool SendTo(SOCKET&);
		static void Receive(SOCKET&); //受信パケットに応じたPACKETのファクトリでもあるが受信PACKETは自己消滅なので外部にインスタンスが見えることはない
	protected:
		struct HEADER{
			unsigned short type;
			unsigned short length;
			unsigned short sequence;
			unsigned short _reserved;
		}__attribute__((packed));
		PACKET(TYPE type, unsigned short size, void* buff);
		PACKET(const HEADER& h, void* buff) :
			header(h), body(buff), length(h.length){};
		virtual ~PACKET(){};
	private:
		static unsigned short sequence;
		virtual void Do(SOCKET&)=0; //パケット処理
		HEADER header;
		void* const body;
		const short length;
	};





	class IDENT : public PACKET{
	public:
		IDENT() : PACKET(PACKET::Ident, 0, 0){};
		IDENT(HEADER& h) : PACKET(h, 0){};
	private:
		static const int maxNameLen = 32;
		static const char* myName;
		char name[maxNameLen];
		void Do(SOCKET&);
	};











	class FEATURECHECK : public PACKET{
	public:
		FEATURECHECK() : PACKET(PACKET::FeatureCheck, 0, 0){};
		FEATURECHECK(HEADER& h) : PACKET(h, 0){};
	private:
		void Do(SOCKET&);
	};

	class FEATUREANSWER : public PACKET{
	public:
		FEATUREANSWER(unsigned ans) :
			PACKET(PACKET::FeatureAnswer,
			       sizeof(unsigned),
			       &(*this).features), features(ans){};
		FEATUREANSWER(HEADER& h) : PACKET(h, &features){};
	private:
		unsigned features;
		void Do(SOCKET&);
	};

}
