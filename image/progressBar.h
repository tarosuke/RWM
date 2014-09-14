/** progressで指定した割合のプログレスバーイメージ
 * テクスチャに貼り付けて使う
 * 毎フレーム生成せずともUpdateで更新も可能
 */
#pragma once

#include "image.h"


class PROGRESSBAR : public IMAGE{
public:
	PROGRESSBAR(unsigned width, unsigned height, float progress);
	void Update(float progress);
	void Error();
};
