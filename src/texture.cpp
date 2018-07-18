#include "texture.hpp"
#include <SOIL/SOIL.h>
#include <sstream>

namespace View {
	bool Texture::source(const char *fname, bool force) {
		if(!sourced || force) {
			this -> fname = fname;
			return source();
		}
		return sourced;
	}
	bool Texture::source(bool force) {
		if(!created)
			return sourced = false;
		if(sourced && !force) return true;
		auto flags = SOIL_FLAG_INVERT_Y;
		if(created) {
			SOIL_load_OGL_texture(fname, SOIL_LOAD_AUTO, value, flags);
		} else {
			value = SOIL_load_OGL_texture(fname, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, flags);
			if(!value) {
				message = SOIL_last_result();
				return width = height = created = sourced = false;
			}
		}
		return created = sourced = true;
	}
	Texture::Texture(void) {
		glCreateTextures(GL_TEXTURE_2D, 1, &value);
		auto err = glGetError();
		if(err == GL_NO_ERROR) {
			created = true;
		} else {
			std::ostringstream oss;
			oss << err;
			message = oss.str();
		}
	}
	Texture::Texture(const char *fname): Texture() {
		source(fname);
	}
	Texture::~Texture(void) {
		if(glIsTexture(value))
			glDeleteTextures(1, &value);
		value = 0;
	}
}
