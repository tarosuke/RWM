

#include <widget.h>



float Widget::sightX(0);
float Widget::sightY(0);
float Widget::distance;
const float ViewWidget::baseDistance(0.7);


void BranchWidget::Draw(){
	children.Foreach(&Widget::Draw);
}

void BranchWidget::DrawTransparent(){
	children.Reveach(&Widget::DrawTransparent);
}

