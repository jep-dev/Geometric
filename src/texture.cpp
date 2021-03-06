#include "texture.hpp"
#include <IL/ilu.h>
#include <sstream>

namespace View {

#ifdef USE_SOIL
Texture::Texture(const char *fname, unsigned int flags): fname(fname) {
	glGenTextures(1, &value);
	glBindTexture(GL_TEXTURE_2D, value);
	//glCreateTextures(GL_TEXTURE_2D, 1, &value);
	if(GL_TRUE != glIsTexture(value)) {
		line = __LINE__;
		created = sourced = false;
		return;
	}
	auto err = glGetError();
	if(err != GL_NO_ERROR) {
		std::ostringstream oss;
		oss << err;
		status.message = "?"; //oss.str();
		created = sourced = false;
		line = __LINE__;
		return;
	}
	created = true;
	auto result = SOIL_load_OGL_texture(fname, SOIL_LOAD_AUTO, value, flags);
	sourced = true;
	// value = SOIL_load_OGL_texture(fname, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, flags);
	// sourced = true;
	line = __LINE__;
	status.message = SOIL_last_result();
}

#elif defined USE_DEVIL

Texture::Texture(const char *fname, unsigned int flags):
		fname(fname), flags(flags) {
	glCreateTextures(GL_TEXTURE_2D, 1, &value);
	glBindTexture(GL_TEXTURE_2D, value);
	if(!glIsTexture(value)) {
		created = sourced = false;
		auto err = glGetError();
		std::ostringstream oss;
		oss << err;
		status.message = oss.str();
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
		status.message = "Failed; ";
	}
	status.message += iluErrorString(ilGetError());
}

#else

Texture::Texture(const char *fname, unsigned int flags):
		fname(fname), flags(flags) {
	glCreateTextures(GL_TEXTURE_2D, 1, &value);
	glBindTexture(GL_TEXTURE_2D, value);
	if(!glIsTexture(value)) {
		created = sourced = false;
		auto err = glGetError();
		std::ostringstream oss;
		oss << err;
		status.message = oss.str();
		return;
	}
	created = true;
}

#endif

Texture::~Texture(void) {
	if(glIsTexture(value))
		glDeleteTextures(1, &value);
	value = 0;
}

}
