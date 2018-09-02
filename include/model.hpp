#ifndef MODEL_HPP
#define MODEL_HPP

#include "dual.hpp"
#include <vector>


template<class T>
struct Model;
template<class T>
struct Model {
	std::vector<Model<T>> children;
	std::vector<T> vertices;
	std::vector<unsigned> indices;
	DualQuaternion<T> transform = {1}, accumulated = transform;
	void accumulate(void) {
		//accumulated = transform;
		for(auto & c : children)
			c.accumulate(accumulated);
	}
	void accumulate(DualQuaternion<T> const& d) {
		accumulated = d * transform;
		accumulate();
	}
	Model<T>& add_child(Model<T> && m = {}) {
		children.emplace_back(std::move(m));
		return children.back();
	}
	Model<T>& add_vertices(void) {
		return *this;
	}
	template<class U, class... V>
	Model<T>& add_vertices(U && u, V &&... v) {
		vertices.emplace_back(std::forward<U>(u));
		return add_vertices(std::forward<V>(v)...);
	}
	Model<T> add_indices(void) {
		return *this;
	}
	template<class... V>
	Model<T> add_indices(unsigned u, V &&... v) {
		indices.emplace_back(u);
		return add_indices(std::forward<V>(v)...);
	}
};


#endif
