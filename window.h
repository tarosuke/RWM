/******************************************************* window handler:window
 * -DTESTの時はrootになるウインドウを開いてそれをroot扱い。
 * でなければrootを取得してそれをrootとする。
 * rootに対してXCompositRedirectSubWindowsして内容を取得
 * また、窓に対してxdamegeイベントを受け付けるよう設定。
 */
#ifndef _WINDOW_
#define _WINDOW_

class WINDOW{
public:
private:
	static class ROOT{ //TESTの時は窓を開いて、でなければrootを取得
	public:
		ROOT();
		int GetRoot();
	private:
		int rootWindowID;
	}root;
};

#endif
