

#include <view/view.h>


class Ambient : public VIEW::DRAWER{
public:
	Ambient(VIEW& v);
private:
	void Draw() const;
};

