/********************************************* abstract avatar controler:ghost
 * The 'ghost' means as 'ghost' in 'ghost in the shell'.
 *
 * GHOSTはAVATARの行動を決めるAVATARとは独立のモジュールになっている
 * AVATARとは独立しているのでAVATARと自由に組み合わせる事ができる。
 *
 * GHOSTのインスタンス自体は何も行動を起こさないので実際にはこれを導出し、バリアントを作る。
 *
 * 例えば各種入力機器の入力処理を備えたローカルユーザーのためのUSERGHOST
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
	virtual const class QON& GetHead() const;
	virtual const QON PickHeadHorizDir(float ratio);
	//操作取得
	virtual float GetRotate();
	virtual float GetForwardStep();
	virtual float GetSideStep();
	virtual unsigned GetActions();
};



#endif

