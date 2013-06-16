#include <math.h>
#include <stdio.h>

#include "qon.h"


QON::QON(const double rift[3]){
	InitByRift(rift);
}

QON::QON(const int rift[3], double ratio){
	double rot[3] = {
		ratio * rift[0],
		ratio * rift[1],
		ratio * rift[2]};
	InitByRift(rot);
}

void QON::InitByRift(const double rift[3]){
	const double x(rift[0]), y(rift[1]), z(rift[2]);
	const double angle(sqrt(x * x + y * y + z * z));
	if(0.0 <angle){
		const double halfAngle(angle * 0.5);
		const double sinA(sin(halfAngle) / angle);
		w = cos(halfAngle);
		i = x * sinA;
		j = y * sinA;
		k = z * sinA;
	}else{
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

void QON::GetRotation(QON::ROTATION& rotation) const{
	const double s(sqrt(1 - (w * w)));
	rotation.x = i / s;
	rotation.y = j / s;
	rotation.z = k / s;
	rotation.angle = acos(w) * 2;
}

QON::QON(const QON::ROTATION& rot){
	const double ha(rot.angle * 0.5);
	const double s(sin(ha));
	w = cos(ha);
	i = rot.x * s;
	j = rot.y * s;
	k = rot.z * s;
}

QON::QON(const VQON& f, const VQON& t){
	const double in(f.In(t)); //内積(差分の回転角)
	const VQON ex(f.Ex(t)); //外積(差分の回転軸)
	const double ha(acos(in) * 0.5);
	const double s(sin(ha));
// printf("ha:%lf.\n", ha);
	w = cos(ha);
	i = ex.i * s;
	j = ex.j * s;
	k = ex.j * s;
}

void QON::print(const char* label) const{
	printf("%s:%lf; %lf, %lf, %lf.\n", label, w, i, j, k);
}



VQON::VQON(const double vector[3]) :
	QON(0, vector[0], vector[1], vector[2]){}

void VQON::ReverseRotate(const QON& by){
	QON r(by.w, -by.i, -by.j, -by.k);
// by.print("P");
// print("Q");
// r.print("R");
	Identifize();

	r *= *this;
	r *= by;

	w = r.w;
	i = r.i;
	j = r.j;
	k = r.k;
	Identifize();
}

void VQON::Rotate(const QON& by){
	QON p(by);
	QON r(p.w, -p.i, -p.j, -p.k);

	Identifize();

	p *= *(QON*)this;
	p *= r;

	w = p.w;
	i = p.i;
	j = p.j;
	k = p.k;

	Identifize();
}

double VQON::Length() const{
	return sqrt(i*i + j*j + k*k);
}

void VQON::Identifize(){
	const double r(1.0 / Length());
	i *= r;
	j *= r;
	k *= r;
}



