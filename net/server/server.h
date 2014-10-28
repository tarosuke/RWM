#pragma once


#include "../net.h"

namespace NET{

	class Server : public Node{
	};

}

//鯖はclientと違って接続時にスレッドを起こす(消滅処理はdereteしたあとでdeleteした人が)
