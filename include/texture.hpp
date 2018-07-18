#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "view.hpp"
#include <IL/il.h>

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
		GLuint value;
		bool created = false, sourced = false;
		ILint width = 0, height = 0;
		std::string message;
		Texture(const char*, unsigned int = 0);
		virtual ~Texture(void);
	};
}

#endif
