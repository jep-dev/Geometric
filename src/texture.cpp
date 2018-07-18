#include "texture.hpp"
#include <IL/ilu.h>
#include <sstream>

namespace View {
	Texture::Texture(const char *fname, unsigned int flags): fname(fname) {
		/*value = 0;
		glGenTextures(1, &value);
		//glCreateTextures(GL_TEXTURE_2D, 1, &value);
		if(!glIsTexture(value)) {
			line = __LINE__;
			created = sourced = false;
			return;
		}
		auto err = glGetError();
		if(err != GL_NO_ERROR) {
			std::ostringstream oss;
			oss << err;
			message = "?"; //oss.str();
			created = sourced = false;
			line = __LINE__;
			return;
		}
		glBindTexture(GL_TEXTURE_2D, value);
		created = true;
		auto result = SOIL_load_OGL_texture(fname, SOIL_LOAD_AUTO, value, flags);*/
		value = SOIL_load_OGL_texture(fname, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, flags);
		sourced = true;
		//sourced = result;
		line = __LINE__;
		message = SOIL_last_result();
	}
	/*Texture::Texture(const char *fname, unsigned int flags):
			fname(fname), flags(flags) {
		glCreateTextures(GL_TEXTURE_2D, 1, &value);

		if(!glIsTexture(value)) {
			created = sourced = false;
			auto err = glGetError();
			std::ostringstream oss;
			oss << err;
			message = oss.str();
			return;
		}

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
	}*/
	Texture::~Texture(void) {
		if(glIsTexture(value))
			glDeleteTextures(1, &value);
		value = 0;
	}
}
