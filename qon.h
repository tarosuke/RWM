#ifndef _QON_
#define _QON_

//クオータニオン
class QON{
public:
	QON() : w(1), i(0), j(0), k(0){};
	QON(double i, double j, double k); //ベクタでクオータニオンを初期化
	QON(double w, double i, double j, double k);
	QON(const double caldan[3]); //カルダン角でクオータニオンを初期化
	QON(const int caldan[3], double ratio); //同上(ただしintから)
	void Rotate(const double caldan[3]); //カルダン角(xyzのオイラー角)で回転
	void operator*=(const QON&); //クオータニオンを「乗算」
	struct ROTATION{
		double angle;
		double x;
		double y;
		double z;
	};
	void GetRotation(ROTATION&);
// private:
	// (w; i, j, k)
	double w;
	double i;
	double j;
	double k;
	void InitByCaldan(const double caldan[3]); //カルダン角で初期化
};

#endif
