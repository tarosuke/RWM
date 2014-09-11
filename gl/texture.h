#pragma once


class IMAGE;
namespace GL{
	class TEXTURE{
// 		TEXTURE();
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
		struct PARAMS{
			int wrap_s;
			int wrap_t;
			int filter_mag;
			int filter_min;
			int texture_mode;
			bool pointSprite;
		};
		TEXTURE(const PARAMS& p=defaultParams);
		TEXTURE(unsigned w,
			unsigned h,
			bool alpha=false,
			const PARAMS& p=defaultParams);
		TEXTURE(const IMAGE&, const PARAMS& p=defaultParams);
		~TEXTURE();

		void Assign(const IMAGE&, const PARAMS& p=defaultParams);
		void Update(const IMAGE&, int x, int y);
	private:
		const unsigned tid;
		bool empty;
		void SetupAttributes(const PARAMS&);

		static const PARAMS defaultParams;
	};
}

