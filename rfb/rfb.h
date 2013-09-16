/******************************************************** RFB protocol handler
 *
 */
#ifndef _RFB_
#define _RFB_

#include <pthread.h>


class RFB{
	RFB();
public:
	class UPDATERECEIVER{
	public:
		enum FORMAT{
			RGBZ,
			BGRZ,
			ZRGB,
			ZGBR
		};
		virtual void Setup(int w, int h, FORMAT)=0;
		virtual void Update(int x, int y, int w, int h, void*)=0;
	};
	RFB(UPDATERECEIVER&);
	~RFB();
private:
	struct PIXFORMAT{
		unsigned char bpp;
		unsigned char luminosity;
		unsigned char bigEndian;
		unsigned char trueColor;
		unsigned short maxRed;
		unsigned short maxGreen;
		unsigned short maxBlue;
		unsigned char shiftRed;
		unsigned char shiftGreen;
		unsigned char shiftBlue;
		unsigned char pad[3];
	}__attribute__((packed));

	static const char* const versionString;
	static const char* const remoteName;
	static const PIXFORMAT formats[];

	//フレームバッファ情報
	int width;
	int height;
	char* name;
	UPDATERECEIVER& receiver;
	UPDATERECEIVER::FORMAT easyFormat;
	const PIXFORMAT* pixFormat;
	void* buffer;

	//接続関連
	const int sock;
	void Read(void* buff, int size);
	void* Read(int size); //領域を確保してread。終了時はfreeすること
	void Write(const void* buff, int size);

	//プロトコルステートマシン
	pthread_t RFBThread;
	void (RFB::*sequencer)(); //プロトコルシーケンサー
	void Seq(){ (this->*sequencer)(); };
	static void* RFBEntry(void*);
	void RFBProtocolVersion();
	void RFBSecurity();
	void RFBServerInit();
};



#endif
