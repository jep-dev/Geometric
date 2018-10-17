#ifndef ATTRIBUTE_HPP
#define ATTRIBUTE_HPP

#include "view.hpp"

namespace View {

struct GlslAttribute;

template<SDL_GLattr A>
struct Attribute;

using Major = Attribute<SDL_GL_CONTEXT_MAJOR_VERSION>;
using Minor = Attribute<SDL_GL_CONTEXT_MINOR_VERSION>;
using Mask = Attribute<SDL_GL_CONTEXT_PROFILE_MASK>;
using DoubleBuffer = Attribute<SDL_GL_DOUBLEBUFFER>;
using AlphaSize = Attribute<SDL_GL_ALPHA_SIZE>;
using RedSize = Attribute<SDL_GL_RED_SIZE>;
using GreenSize = Attribute<SDL_GL_GREEN_SIZE>;
using BlueSize = Attribute<SDL_GL_BLUE_SIZE>;

int get_attr(SDL_GLattr attr) {
	int out;
	SDL_GL_GetAttribute(attr, &out);
	return out;
}
void set_attr(SDL_GLattr key, int value) {
	SDL_GL_SetAttribute(key, value);
}
template<class T>
void set_attr(T const& t) {
	set_attr(T::key, t.value);
}
template<class... T>
void set_attrs(T &&... t) {}
template<class S, class... T>
void set_attrs(S && s, T &&... t) {
	set_attr(s);
	set_attrs(std::forward<T>(t)...);
}
template<class... T>
void set_attrs(SDL_GLattr key, int value, T &&... t) {
	set_attr(key, value);
	set_attrs(std::forward<T>(t)...);
}
bool ignore_attr(int, int) { return true; }
bool ignore_attr(SDL_GLattr, int) { return true; }
bool match_attr(int lhs, int rhs) { return lhs == rhs; }
bool meet_attr(int lhs, int meet) { return lhs >= meet; }
bool match_attr(SDL_GLattr key, int in) { return match_attr(in, get_attr(key)); }
bool mask_attr(int lhs, int mask) { return match_attr(lhs & mask, mask); }
bool mask_attr(SDL_GLattr key, int in) { return mask_attr(get_attr(key), in); }

//template<SDL_GLattr A, class CMP>
template<SDL_GLattr A>
struct Attribute {
	typedef SDL_GLattr key_type;
	typedef int value_type;
	//typedef CMP compare_type;
	typedef bool (compare_type) (int, int);

	static constexpr key_type key = A;
	int value;
	//compare_type compare;
	compare_type *compare = &match_attr;

	template<class S>
	int set(S const& s) { return value = s; }
	int set(void) const { return value; }
	int get(void) const { return get_attr(A); }
	operator bool(void) const { return compare(set(), get()); }
	bool operator&&(bool const& rhs) const { return bool(*this) && rhs; }
	bool operator||(bool const& rhs) const { return bool(*this) || rhs; }
	template<SDL_GLattr B>
	bool operator&&(Attribute<B> const& rhs) const { return *this && bool(rhs); }
	template<SDL_GLattr B>
	bool operator||(Attribute<B> const& rhs) const { return *this || bool(rhs); }
	template<class S>
	S& operator>>(S &s) { return s = value; }
};
/** Makes an attribute out of the given value-comparator pair, but can't form one
 * out of a reference (requires refactoring to support captures) */
template<SDL_GLattr A, class CMP>
auto make_attr(int value, CMP && comp = {})
	-> Attribute<A> { return { value, std::forward<CMP>(comp) }; };

/* TODO list:
 *   take a page from <functional>; how to compose/curry?
 *   fix false negative for M>3, N<3 (e.g. 4.0) which evaluates as false.
 *    - Going by semantic versioning, this would not be a false negative
 *      (major version changes introduce backward-incompatible changes.)
 */

struct GlslAttribute {
	const char *name;
	GLint location;
	operator GLint(void) const {
		return location;
	}
	GlslAttribute& enable(void) {
		return glEnableVertexAttribArray(location), *this;
	}
	GlslAttribute& disable(void) {
		return glDisableVertexAttribArray(location), *this;
	}
	GLint size(void) const {
		GLint s = -1;
		glGetVertexAttribiv(location, GL_VERTEX_ATTRIB_ARRAY_SIZE, &s);
		return s;
	}
	GLint stride(void) const {
		GLint s = -1;
		glGetVertexAttribiv(location, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &s);
		return s;
	}
	GLenum type(void) const {
		GLenum e;
		glGetVertexAttribiv(location, GL_VERTEX_ATTRIB_ARRAY_TYPE, &e);
		return e;
	}
	GlslAttribute& setName(const char *n) {
		return name = n, *this;
	}

	GlslAttribute& setPointer(GLuint count, GLenum type, GLboolean normalized,
		GLsizei stride, const void* ptr) {
		glVertexAttribPointer(location, count, type, normalized, stride, ptr);
	}

	GlslAttribute& setLocation(GLuint program, GLint loc) {
		glBindAttribLocation(program, loc, name);
		location = glGetAttribLocation(program, name);
		enable();
		return *this;
	}

	GlslAttribute(const char *name): name(name) {}

	/*template<class S, class... T>
	GlslAttribute(const char *name, S && s, T &&... t):
			GlslAttribute(name) {
		setLocation(std::forward<S>(s), std::forward<T>(t)...);
	}*/
	GlslAttribute(const char *name, GLuint program, GLint loc):
			GlslAttribute(name) {
		setLocation(program, loc);
	}
	GlslAttribute(const char *name, GLuint program, GLint loc,
			GLuint count, GLenum type, GLboolean normalized,
			GLsizei stride, const void *ptr):
				GlslAttribute(name, program, loc) {
		setPointer(count, type, normalized, stride, ptr);
	}

};
}


#endif
