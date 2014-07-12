#pragma once

namespace GL{

	//ディスプレイリストのラッパ
	class DisplayList{
	public:
		//記録用RAIIキー(存在している間は記録)
		class Recorder{
		public:
			Recorder(DisplayList& dl, bool draw = false) :
				recorder(dl){ dl.StartRecord(draw); };
			~Recorder(){ recorder.EndRecord(); };
		private:
			DisplayList& recorder;
		};

		DisplayList();
		~DisplayList();
		unsigned GetID(){ return id; };
		void Playback();
	private:
		static bool recording;
		const unsigned id;

		void StartRecord(bool);
		void EndRecord();
	};

}
