///////////////////////////////////////////////////////////// ABSTRACT DISPLAY
#include "display.h"

#include "view.h"


DISPLAY::DISPLAY(const PARAM& p) : view(VIEW::New(p.width, p.height)){
}
