

#include "event.h"
#include "window.h"


void MOUSE_DOWN_EVENT::Handle(WINDOW& w)const{ w.OnMouseDown(*this); };
void MOUSE_UP_EVENT::Handle(WINDOW& w)const{ w.OnMouseUp(*this); };
void MOUSE_MOVE_EVENT::Handle(WINDOW& w)const{ w.OnMouseMove(*this); };
void MOUSE_ENTER_EVENT::Handle(WINDOW& w)const{ w.OnMouseEnter(*this); };
void MOUSE_LEAVE_EVENT::Handle(WINDOW& w)const{ w.OnMouseLeave(*this); };

void KEY_DOWN_EVENT::Handle(WINDOW& w)const{ w.OnKeyDown(*this); };
void KEY_REPEAT_EVENT::Handle(WINDOW& w)const{ w.OnKeyRepeat(*this); };
void KEY_UP_EVENT::Handle(WINDOW& w)const{ w.OnKeyUp(*this); };

