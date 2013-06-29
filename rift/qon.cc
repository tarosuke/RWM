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
	Normalize();
}

void QON::GetRotation(QON::ROTATION& rotation) const{
	const double s(sqrt(1 - (w * w)));
	rotation.x = i / s;
	rotation.y = j / s;
	rotation.z = k / s;
	rotation.angle = acos(w) * 2;
}

QON::QON(const QON::ROTATION& rot){
	SetRotation(rot);
}

void QON::SetRotation(const ROTATION& rot){
	const double ha(rot.angle * 0.5);
	const double s(sin(ha));
	w = cos(ha);
	i = rot.x * s;
	j = rot.y * s;
	k = rot.z * s;
}

void QON::GetMatrix(double matrix[16]){
// 	const double ww(w * w * 2);
	const double xx(i * i * 2);
	const double yy(j * j * 2);
	const double zz(k * k * 2);
	const double xy(i * j * 2);
	const double yz(j * k * 2);
	const double zx(k * i * 2);
	const double wx(w * i * 2);
	const double wy(w * j * 2);
	const double wz(w * k * 2);
	matrix[0] = 1.0 - yy - zz;
	matrix[1] = xy + wz;
	matrix[2] = zx - wy;
	matrix[3] = 0.0;
	matrix[4] = xy - wz;
	matrix[5] = 1.0 - zz - xx;
	matrix[6] = yz + wx;
	matrix[7] = 0.0;
	matrix[8] = zx + wy;
	matrix[9] = yz - wx;
	matrix[10]= 1.0 - xx - yy;
	matrix[11] = matrix[12] = matrix[13] = matrix[14] = 0.0;
	matrix[15] = 1.0;
}

QON::QON(const VQON& f, const VQON& t){
	VQON ff(f);
	VQON tt(t);
	ff.Normalize();
	tt.Normalize();

	const double in(tt.In(ff)); //内積(差分の回転角)
	VQON ex(tt.Ex(ff)); //外積(差分の回転軸)

	const double c(sqrt(0.5 * (1 + in)));
	const double s(sqrt(0.5 * (1 - in)));
	w = c;
	i = s * ex.i;
	j = s * ex.j;
	k = s * ex.k;
	Normalize();
}

void QON::print(const char* label) const{
	printf("%s(%lf):% lf; % lf, % lf, % lf.\n", label, sqrt(i*i+j*j+k*k), w, i, j, k);
}

double QON::Length() const{
	return sqrt(w*w + i*i + j*j + k*k);
}

void QON::Normalize(){
	const double r(1.0 / Length());
	w *= r;
	i *= r;
	j *= r;
	k *= r;
}



VQON::VQON(const double vector[3]) :
	QON(0, vector[0], vector[1], vector[2]){}

void VQON::ReverseRotate(const QON& by){
	QON r(by.w, -by.i, -by.j, -by.k);

	r *= *this;
	r *= by;

	w = 0.0;
	i = r.i;
	j = r.j;
	k = r.k;
	Normalize();
}

void VQON::Rotate(const QON& by){
	QON p(by);
	QON r(p.w, -p.i, -p.j, -p.k);

	p *= *this;
	p *= r;

	w = 0.0;
	i = p.i;
	j = p.j;
	k = p.k;
	Normalize();
}

double VQON::Length() const{
	return sqrt(i*i + j*j + k*k);
}

void VQON::Normalize(){
	const double r(1.0 / Length());
	w = 0;
	i *= r;
	j *= r;
	k *= r;
}



