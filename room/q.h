#ifndef _Q_
#define _Q_

template<class T> class Q{
public:
	Q() : first(0){};
	void Unload(){
		while(first){
			delete first;
		}
	};
	~Q(){
		Unload();
	};
	class NODE{
		NODE();
	public:
		NODE(Q& q) : first(q.first){
			next = first;
			first = this;
		};
		virtual ~NODE(){
			first = next;
		};
	private:
		NODE*& first;
		NODE* next;
	};
private:
	NODE* first;
};

#endif
