#ifndef _QON_
#define _QON_

//クオータニオン
class QON{
	friend class VQON;
public:
	QON() : w(1), i(0), j(0), k(0){};
	QON(const double caldan[3]); //カルダン角でクオータニオンを初期化
	QON(const int caldan[3], double ratio); //同上(ただしintから)
	void Rotate(const double caldan[3]); //カルダン角(xyzのオイラー角)で回転
	void operator*=(const QON&); //クオータニオンを「乗算」
	void operator*=(const double); //回転角を乗算
	struct ROTATION{
		double angle;
		double x;
		double y;
		double z;
	};
	void GetRotation(ROTATION&);
// protected:
	// (w; i, j, k)
	double w;
	double i;
	double j;
	double k;
	void InitByCaldan(const double caldan[3]); //カルダン角で初期化
	QON(double w, double x, double y, double z) :
	w(w), i(x), j(y), k(z){};
};


//ベクタの四元数
class VQON : public QON{
	friend class QON;
public:
	VQON() : QON(0, 0, 0, 0){};
	VQON(double x, double y, double z) : QON(0, x, y, z){};
	VQON(const double vector[3]); //ベクタでクオータニオンを初期化
	VQON(const int vector[3], double ratio) : //同上(ただしintから)
		QON(0, ratio * vector[0], ratio * vector[1], ratio * vector[2]){};
	void Rotate(const QON&); //四元数で回転
	void ReverseRotate(const QON&); //四元数で逆回転
	double operator*(const VQON&) const; //内積
	VQON Multiply(const VQON&) const; //外積
	void GetVector(double vector[3]) const; //i,j,kを取得
};


#endif
