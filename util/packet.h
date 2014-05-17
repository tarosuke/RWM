#pragma once

#include "socket.h"



namespace PACKET{


	class PACKET{
	public:
		enum TYPE{
			Ident,
			FeatureCheck, //追加機能チェック
			FeatureAnswer, //追加機能応答
			Form, //形状データ設定
			StartRegisterPersistents, //恒久データ登録開始
			StopRegisterPersitents, //恒久データ登録終了、有効化
			FrameSync, //フレーム同期
			RenderForms, //レンダリング対象形状指定
			FinishRender, //形状データ指定終了
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
