

#include <view/view.h>


class Ambient : public VIEW::DRAWER{
public:
	//描画オブジェクトの抽象クラス
	class Object{
	public:
		Object() : node(*this){};
		virtual void Draw() const;
	protected:
	private:
		TOOLBOX::NODE<Object> node;

	};
	Ambient(VIEW& v);
private:
	TOOLBOX::QUEUE<Object> gates;
	TOOLBOX::QUEUE<Object> objects;
	TOOLBOX::QUEUE<Object> transparents;
	void Draw() const;
};

