#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "view.hpp"
#include <SOIL/SOIL.h>

namespace View {
	struct Texture {
		/*
		const char *fname = "";
		GLuint value = 0;
		int width = 0, height = 0;
		bool created = false, sourced = false;
		std::string message;
		bool source(bool force = true);
		bool source(const char*, bool force = true);
		Texture(void);
		Texture(const char *);
		*/
		const char *fname;
		unsigned int flags;
		GLuint value = SOIL_load_OGL_texture(fname, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, flags);
		std::string message = SOIL_last_result();
		Texture(const char*, unsigned int = SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		virtual ~Texture(void);
	};
}

#endif
