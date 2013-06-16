#ifndef _QON_
#define _QON_

//クオータニオン
class VQON;
class QON{
	friend class VQON;
public:
	QON() : w(1), i(0), j(0), k(0){};
	QON(const double rift[3]); //riftの値でクオータニオンを初期化
	QON(const int rift[3], double ratio); //同上(ただしintから)
	void Rotate(const double rift[3]); //riftの値で回転
	void operator*=(const QON&); //クオータニオンを「乗算」
	void operator*=(const double t){ //回転角を乗算
		w *= t;
	};

	//NOTE:Getはriftの値ではなく自由軸回転
	struct ROTATION{
		double angle;
		double x;
		double y;
		double z;
	};
	void GetRotation(ROTATION&) const;
	QON(const ROTATION&); //自由軸回転で初期化

	QON(const VQON&, const VQON&); //二つの単位ベクタの差分で初期化
	QON operator-(){
		QON r(w, -i, -j, -k);
		return r;
	};
	void print(const char* label) const;
// protected:
	// (w; i, j, k)
	double w;
	double i;
	double j;
	double k;
	void InitByRift(const double rift[3]); //riftの値で初期化
	QON(double w, double x, double y, double z) : //即値で初期化
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
	void GetVector(double vector[3]) const; //i,j,kを取得
	double Length() const; //ベクトルの長さ
	void Identifize(); //長さを1にする
	double In(const VQON& t) const{ //内積
		return i*t.i + j*t.j + k*t.k;
	};
	VQON Ex(const VQON& t) const{ //外積
		const VQON r(j*t.k - k*t.j, k*t.i - i*t.k, i*t.j - j*t.i);
		return r;
	};
};


#endif
