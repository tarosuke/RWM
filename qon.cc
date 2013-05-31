#include <math.h>

#include "qon.h"
#include <gl-matrix.h>

QON::QON(double w, double i, double j, double k) :
	w(w), i(i), j(j), k(k){}

QON::QON(double i, double j, double k) :
	w(0), i(i), j(j), k(k){}

QON::QON(const double caldan[3]){
	InitByCaldan(caldan);
}

QON::QON(const int caldan[3], double ratio){
	double rot[3] = {
		ratio * caldan[0],
		ratio * caldan[1],
		ratio * caldan[2]};
	InitByCaldan(rot);
}

void QON::InitByCaldan(const double caldan[3]){
	const double x(caldan[0]), y(caldan[1]), z(caldan[2]);
	const double angle(sqrt(x * x + y * y + z * z));
	const double halfAngle(angle * 0.5);
	const double sinA(sin(halfAngle) / angle);
	w = cos(halfAngle);
	i = x * sinA;
	j = y * sinA;
	k = z * sinA;
}

void QON::operator*=(const QON& er){
	QON ed(*this);
#if 1
#if 1
	*this = er;
	w *= 100;
#else
	w = ed.w * er.w - ed.i * er.i - ed.j * er.j - ed.k * er.k;
	i = ed.i * er.w + ed.w * er.i + ed.j * er.k - ed.k * er.j;
	j = ed.j * er.w + ed.w * er.j + ed.k * er.i - ed.i * er.k;
	k = ed.k * er.w + ed.w * er.k + ed.i * er.j - ed.j * er.i;
#endif
#else
	double src[4] = { i, j, k, w };
	double dst[4] = { er.i, er.j, er.k, er.w };
	quat_multiply(src, dst, 0);
	w = src[3];
	i = src[0];
	j = src[1];
	k = src[2];
#endif
/*
	double qax = quat[0], qay = quat[1], qaz = quat[2], qaw = quat[3],
	qbx = quat2[0], qby = quat2[1], qbz = quat2[2], qbw = quat2[3];

	dest[0] = qax * qbw + qaw * qbx + qay * qbz - qaz * qby;
	dest[1] = qay * qbw + qaw * qby + qaz * qbx - qax * qbz;
	dest[2] = qaz * qbw + qaw * qbz + qax * qby - qay * qbx;
	dest[3] = qaw * qbw - qax * qbx - qay * qby - qaz * qbz;*/
}
