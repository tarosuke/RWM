#include <GL/glew.h>
#include <GL/gl.h>

#include <assert.h>

#include "displayList.h"


namespace GL{

	bool DisplayList::recording(false);
	DisplayList::DisplayList() : id(glGenLists(1)){};
	DisplayList::~DisplayList(){
		glDeleteLists(id, 1);
	};
	void DisplayList::Playback() const{
		glCallList(id);
	}
	void DisplayList::StartRecord(bool draw){
		assert(!recording);
		recording = true;
		glNewList(id, draw ? GL_COMPILE_AND_EXECUTE : GL_COMPILE);
	}
	void DisplayList::EndRecord(){
		glEndList();
		recording = false;
	}

}
