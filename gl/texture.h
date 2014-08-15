#pragma once


class IMAGE;
namespace GL{
	class TEXTURE{
		TEXTURE(const TEXTURE&);
		void operator=(const TEXTURE&);
	public:
		class BINDER{
			BINDER();
			BINDER(const BINDER&);
			void operator=(const BINDER&);
		public:
			BINDER(const TEXTURE&);
			~BINDER();
		private:
			static unsigned lastBinded;
			const unsigned prevBinded;
		};
		TEXTURE();
		TEXTURE(unsigned w, unsigned h, bool alpha=false);
		TEXTURE(const IMAGE&);
		~TEXTURE();

		void Assign(const IMAGE&);
		void Update(const IMAGE&, int x, int y);
	private:
		const unsigned tid;
		bool empty;
		void SetupAttributes();
	};
}

