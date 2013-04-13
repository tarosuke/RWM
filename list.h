
#ifndef _LIST_
#define _LIST_

template<class T> class LIST{
public:
	class NODE{
	public:
		void Add(NODE& node){
			node.next = &q;
			node.prev = q.prev;
			q.prev = &node;
			(*prev).next = &node;
		};
		void Remove(){
			(*prev).next = next;
			(*next).prev = prev;
			prev = next = this;
		};
		NODE() : prev(0), next(0){};
	protected:
		T* prev;
		T* next;
	};
	class ITOR{
	public:
		ITOR() : nowOn(&q){};
		operator T*(){
			return nowOn;
		};
		T* operator++(){
			if(nowOn){
				if(nowOn == &q){
					nowOn = 0;
				}else{
					nowOn = (*nowOn).next;
				}
			}
			return nowOn;
		};
		T* operator++(int){
			if(nowOn){
				if(nowOn == &q){
					nowOn = 0;
				}else{
					T* const r(nowOn);
					nowOn = (*nowOn).next;
					return r;
				}
			}
			return nowOn;
		};
	private:
		T* nowOn;
	};
private:
	NODE q;
};

#endif
