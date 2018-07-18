#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "view.hpp"

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
		GLuint value;
		std::string message;
		Texture(const char*);
		virtual ~Texture(void);
	};
}

#endif
