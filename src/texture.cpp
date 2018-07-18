#include "texture.hpp"
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
		glCreateTextures(GL_TEXTURE_2D, 1, &value);
		std::ostringstream oss;
		sourced = false;
		auto err = glGetError();
		oss << err;
		switch(glGetError()) {
			case GL_NO_ERROR:
				message = "";
				sourced = true;
				break;
			default: break;
			/*case GL_INVALID_ENUM:
			case GL_INVALID_VALUE:*/
		}
		if(!sourced) message = oss.str();
		return sourced;
	}
	Texture::Texture(void): fname("") {}
	Texture::Texture(const char *fname): fname(fname) {
	}
}
