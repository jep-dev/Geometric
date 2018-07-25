#ifndef PRESENTER_HPP
#define PRESENTER_HPP

#include "frame.hpp"
#include "model.hpp"

template<class S>
struct Presenter;

template<class S>
struct Presenter: Events::Handler<S> {
	using Events::Handler<S>::operator();
	View::Frame frame;
	Model model;

	View::GLfloat projection[16] = {0};
	S& project(View::GLuint dest, float right, float top, float near, float far) {
		using namespace View;
		float mx = near / right, mz = near / top, mwy = -1,
			dy = near - far,
			myy = (far + near) / dy,
			myw = 2 * near * far / dy;
		projection[0] = mx;
		projection[6] = -myy;
		projection[7] = -mwy;
		projection[9] = mz;
		projection[14] = myw;
		glUniformMatrix4fv(0, 1, GL_FALSE, projection);
		return static_cast<S&>(*this);
	}
	auto draw(void) -> decltype(frame.draw()) { return frame.draw(); }
	auto clear(void) -> decltype(frame.clear()) { return frame.clear(); }
	auto getWindow(void) -> decltype(frame.getWindow()) { return frame.getWindow(); }
	auto getContext(void) -> decltype(frame.getContext()) { return frame.getContext(); }

	template<class... T>
	Presenter(T &&... t): frame(std::forward<T>(t)...) {}
};

#endif
