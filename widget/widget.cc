

#include <widget.h>



float Widget::sightX(0);
float Widget::sightY(0);


void BranchWidget::Draw(){
	children.Foreach(&Widget::Draw);
}

void BranchWidget::DrawTransparent(){
	children.Reveach(&Widget::DrawTransparent);
}

