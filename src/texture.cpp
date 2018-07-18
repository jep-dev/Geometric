#include "texture.hpp"
#include <IL/ilu.h>
#include <sstream>

namespace View {
	/*bool Texture::source(const char *fname, bool force) {
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
	*/
	Texture::Texture(const char *fname, unsigned int flags):
			fname(fname), flags(flags) {
		glCreateTextures(GL_TEXTURE_2D, 1, &value);
		/*if(!glIsTexture(value)) {
			created = sourced = false;
			auto err = glGetError();
			std::ostringstream oss;
			oss << err;
			message = oss.str();
			return;
		}*/
		created = true;
		ilInit();
		ILuint image;
		ilGenImages(1, &image);
		ilBindImage(image);
		ilLoadImage(fname);
		auto err = ilGetError();
		if(err == IL_NO_ERROR) {
			sourced = true;
			ilGetIntegerv(IL_IMAGE_WIDTH, &width);
			ilGetIntegerv(IL_IMAGE_HEIGHT, &height);
			auto data = ilGetData();
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_INT, data);
			ilDeleteImages(1, &image);
		} else {
			message = "Failed; ";
		}
		message += iluErrorString(ilGetError());
	}
	Texture::~Texture(void) {
		if(glIsTexture(value))
			glDeleteTextures(1, &value);
		value = 0;
	}
}
