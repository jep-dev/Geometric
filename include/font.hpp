#ifndef FONT_HPP
#define FONT_HPP

#include <FTGL/ftgl.h>
#include "events.hpp"

namespace Fonts {
	using namespace Events;
	template<class F = FTGLPixmapFont>
	struct Font {
		const char *path;
		F font;
		Status status;
		bool Error(void) const {
			return !font.Error();
		}
		template<class S = int>
		Font(const char *path, S s = 72): path(path), font(path),
				status{font.Error() ? StatusError : StatusPass} {
			font.FaceSize(s);
		}
	};
	FTGLPixmapFont getFont(const char *path, unsigned size, bool *result = nullptr) {
		bool res;
		if(!result) result = &res;
		FTGLPixmapFont out(path);
		if(out.Error()) {
		}
	}
}

#endif
