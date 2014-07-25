/******************************************************** adstract headtracker
 *
 */
#pragma once

#include <toolbox/glpose/glpose.h>


class HEADTRACKER : public GLPOSE{
public:
	//TODO:HEADTRACKERそれ自体の構築子は隠してRIFTとかとのファクトリメソッドが必要
	HEADTRACKER(){};
	~HEADTRACKER(){};
	//TODO:視線が指定平面のどこを指しているかを返す。現在この機能はWINDOW内にあるが、そうするとWINODWが姿勢を要求するのでWINDOWにあるべきではなく、このクラスかGLPOSEへ移動すべきものだ。あるいは将来的にはGLPOSEから具象的なGLを抜いてより抽象的なPOSEにし、切り出したGLをPOSEへ依存性注入する方がいい。POSEの子を作るのでGLは依存性注入にする必要がある。これは一つのアンチパターンであり、既存言語に不足しているものだ。
protected:
private:
};


