#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "view.hpp"
#include <IL/il.h>
#include <SOIL/SOIL.h>

#include "events.hpp"

namespace View {

struct Texture {
	const char *fname;
	unsigned int flags;
	GLuint value;
	bool created = false, sourced = false;
	unsigned int line = 0;
	ILint width = 0, height = 0;
	Events::Status status;
	Texture(const char*, unsigned int = SOIL_FLAG_INVERT_Y | SOIL_FLAG_MIPMAPS);
	virtual ~Texture(void);
};

}

#endif
