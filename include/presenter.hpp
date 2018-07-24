#ifndef PRESENTER_HPP
#define PRESENTER_HPP

#include "events.hpp"
#include "frame.hpp"
#include "model.hpp"

template<class S>
struct Presenter;
template<class S>
struct Presenter: Events::Handler<S> {
	using Events::Handler<S>::operator();
	View::Frame frame;
	Model model;
	template<class... T>
	Presenter(T &&... t): frame(std::forward<T>(t)...) {}
};

#endif
