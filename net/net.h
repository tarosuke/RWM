#pragma once

#include "../toolbox/queue/queue.h"
#include "../common/thread.h"
#include "socket.h"



namespace NET{

	class Packet{
		Packet(const Packet&);
		void operator=(const Packet&);
	public:
		enum Type{
			OK,
			UNKNOWN,
		};
		struct Handler{
			Type type;
			void (*handler)(Packet&);
		};
		void Receive(SOCKET&);
		void Send(SOCKET&);
		class Node;
		bool Handle(const Handler[]);
		Packet(); //受信するための空パケット準備
		Packet(Type, unsigned len, void* b = 0); //送信するためのパケット
		~Packet();
	private:
		static unsigned sequence;
		TOOLBOX::NODE<Packet> node;
		struct Head{
			unsigned type : 8;
			unsigned seqence : 24;
			unsigned length;
		}__attribute__((packed));
		Head head;
		Packet* result;
		void* body;
	};


	//NOTE:鯖とクライアントは結局同じもので、パケットハンドラが違うだけ
	//スレッドとして動作し、ソケットが閉じられたらthreadを終了しつつdelete this。
	class Node : public THREAD{
		Node();
		Node(const Node&);
		void operator=(const Node&);
	public:
	protected:
		Node(SOCKET& sock, const Packet::Handler[]);
		void Close();
	private:
		SOCKET& sock;
		const Packet::Handler* const handlers;
		unsigned seqence;
		TOOLBOX::QUEUE<Packet> sentPackets; //応答待ち
		bool keep;

		void Thread();
	};



}
