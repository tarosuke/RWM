#pragma once


template<typename T> class FACTORY{
	FACTORY();
	FACTORY(const FACTORY&);
	void operator=(const FACTORY&);
public:
	FACTORY(T* (*f)()) :
		next(start),
		factory(f){
			start = this;
		};
	static T* New(){
		for(FACTORY* f(start); f; f = (*f).next){
			T* const t(((*f).factory)());
			if(t){
				return t;
			}
		}
		return 0;
	};
private:
	static FACTORY* start;
	FACTORY* const next;
	T* (*const factory)();;
};

