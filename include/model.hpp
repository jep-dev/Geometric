#ifndef MODEL_HPP
#define MODEL_HPP

#include "dual.hpp"
#include <vector>

template<class T, class I = unsigned>
struct Model;

template<class T, class I>
struct Model {
	DualQuaternion<T> transform = {1}, accumulated = transform;
	std::vector<Model> children;
	std::vector<T> vertices;
	std::vector<I> indices;
	unsigned size(void) const { return children.size(); }
	DualQuaternion<T>& accumulate(DualQuaternion<T> d = {1}) {
		accumulated = d * transform;
		//accumulated = transform * d;
		for(auto & c : children)
			c.accumulate(accumulated);
		return accumulated;
	}
	Model& emplace_back(DualQuaternion<T> d = {1}) {
		children.emplace_back(std::move(Model{d, d*transform}));
		//children.emplace_back(std::move(Model{d, transform*d}));
		return back();
	}
	typename std::vector<Model>::iterator begin(void) { return children.begin(); }
	typename std::vector<Model>::const_iterator cbegin(void) const { return children.cbegin(); }
	typename std::vector<Model>::iterator end(void) { return children.end(); }
	typename std::vector<Model>::const_iterator cend(void) const { return children.cend(); }

	Model& back(void) { return children.back(); }
	Model& front(void) { return children.front(); }

	Model& add_vertices(void) {
		return *this;
	}
	template<class U, class... V>
	Model& add_vertices(U && u, V &&... v) {
		vertices.emplace_back(std::forward<U>(u));
		return add_vertices(std::forward<V>(v)...);
	}
	Model& add_indices(void) {
		return *this;
	}
	template<class... V>
	Model& add_indices(unsigned u, V &&... v) {
		indices.emplace_back(u);
		return add_indices(std::forward<V>(v)...);
	}
	Model(DualQuaternion<T> transform = {1}):
		Model(transform, transform) {}
	Model(DualQuaternion<T> transform, DualQuaternion<T> accumulated):
		transform(transform), accumulated(accumulated) {}
};

#endif
