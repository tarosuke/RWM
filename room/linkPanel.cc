/******************************************************************* linkpanel
 * 他の部屋への穴
 */
#include "linkPanel.h"

void LINKPANEL::Draw(unsigned follow){
	//ステンシルバッファにだけ描画させる
	glColorMask(0,0,0,0);
	glDepthMask(0);
}

