/********************************************* abstract avatar controler:ghost
 * The 'ghost' means as 'ghost' in 'ghost in the shell'.
 *
 * GHOSTはAVATARの行動を決めるAVATARとは独立のモジュールになっている
 * AVATARとは独立しているのでAVATARと自由に組み合わせる事ができる。
 *
 * そのバリアントは例えば各種入力機器の入力処理を備えたローカルユーザーのためのUSERGHOST
 * さらにOclulus Riftに対応したRIFTGHOST。リモートユーザのためのREMOTEGHOST。
 * 例えばRUBBERDUCHGHOST(別名：うなづきちゃん)のようなNPCのためのGHOSTもある。
 *
 * なお、定期的に呼ばれるという事はないのでバックグラウンドで何か処理が必要なときは、
 * pthreadあるいはforkなどでバックグラウンド処理のためのスレッドを起こす必要がある。
 */
#ifndef _GHOST_
#define _GHOST_


class GHOST{
public:
	//頭操作
	virtual const class QON& GetHead() const=0;
	virtual const QON PickHeadHorizDir(float ratio)=0;
	//操作取得
	virtual float GetRotate()=0;
	virtual float GetForwardStep()=0;
	virtual float GetSideStep()=0;
	virtual unsigned GetActions()=0;
};



#endif

