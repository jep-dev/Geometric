#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "view.hpp"

namespace View {
	struct Texture {
		const char *fname;
		GLuint value;
		bool created = false, sourced = false;
		std::string message;
		bool source(bool force = true);
		bool source(const char*, bool force = true);
		Texture(const char *);
		Texture(void);
		virtual ~Texture(void);
	};
}

#endif
