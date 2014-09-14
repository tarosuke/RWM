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
private:
	static const unsigned char barColor[3];
	static const unsigned char white[3];
	static const unsigned char errorColor[3];
};
