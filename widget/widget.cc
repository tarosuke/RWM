

#include <widget.h>


Widget::Vector Widget::lookingPoint;
float Widget::distance;
double Widget::rotation;
const float ViewWidget::baseDistance(0.7);
const float ViewWidget::motionDistance(0.8);
const float Widget::scale(0.0011);



BranchWidget::~BranchWidget(){
	//子要素の全削除
	Widget* w;
	while((w = children.Get())){
		delete w;
	}
}


void BranchWidget::Draw(){
	children.Foreach(&Widget::Draw);
}

void BranchWidget::DrawTransparent(){
	children.Reveach(&Widget::DrawTransparent);
}

void ViewWidget::Update(const COMPLEX<4>& pose){
	//ロール情報を保存しておく
	COMPLEX<4> p(pose);
	p.FilterAxis(4);
	COMPLEX<4>::ROTATION r;
	p.GetRotation(r);
	rotation = -r.axis[2] * r.angle * 180 / 3.14;

	//仮想画面上の視線の先を算出
	VECTOR<3> viewLine((const double[]){ 0, 0, 1 });
	viewLine.Rotate(pose);
	const double* const v(viewLine);
	if(v[2] <= 0){
		//後ろを向いてるので窓は一切見えない
		lookingFront = false;
		return;
	}
	lookingFront = true;

	//移動監視
	const float vs(1.0 / (v[2] * scale));
	const double lp[] = {
		-v[0] * motionDistance * vs,
		v[1] * motionDistance * vs };
	const Vector newLookingPoint(lp);
	if(newLookingPoint != lookingPoint){
		//移動しているのでイベント発生
	}
	lookingPoint = newLookingPoint;
}

void ViewWidget::Draw(){
	if(lookingFront){
		BranchWidget::Draw();
	}
}

void ViewWidget::DrawTransparent(){
	if(lookingFront){
		BranchWidget::DrawTransparent();
	}
}



