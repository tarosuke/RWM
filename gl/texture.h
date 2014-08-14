#pragma once


class IMAGE;
namespace GL{
	class TEXTURE{
		TEXTURE(const TEXTURE&);
		void operator=(const TEXTURE&);
	public:
		TEXTURE();
		TEXTURE(unsigned w, unsigned h);
		TEXTURE(const IMAGE&);
		~TEXTURE();

		void Bind() const;
		void Assign(const IMAGE&);
		void Update(const IMAGE&, int x, int y);
	private:
		const unsigned tid;
		bool empty;
	};
}

