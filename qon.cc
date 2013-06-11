#include <math.h>
#include <stdio.h>

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
if(0.0 <angle){

	const double halfAngle(angle * 0.5);
	const double sinA(sin(halfAngle) / angle);
	w = cos(halfAngle);
	i = x * sinA;
	j = y * sinA;
	k = z * sinA;

}else{
	printf("not rotated.\n");
	w = 1;
	i = j = k = 0;
}
}

void QON::operator*=(const QON& er){
	QON ed(*this);
	w = ed.w * er.w - ed.i * er.i - ed.j * er.j - ed.k * er.k;
	i = ed.i * er.w + ed.w * er.i + ed.j * er.k - ed.k * er.j;
	j = ed.j * er.w + ed.w * er.j + ed.k * er.i - ed.i * er.k;
	k = ed.k * er.w + ed.w * er.k + ed.i * er.j - ed.j * er.i;
}
