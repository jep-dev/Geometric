#include <iostream>

#include "model.hpp"
#include "dual.tpp"

int main(int argc, const char *argv[]) {
	typedef float T;
	typedef DualQuaternion<T> DQ;

	Model<T> parent;
	parent.transform = {1, 0, 0, 0, 0, 1, 0, 0};
	Model<T>& child = parent.add_child();
	child.transform = {0, 1};
	Model<T>& leaf = child.add_child();

	child.transform = {1, 0, 0, 0, 0, 1, 0, 0};
	parent.transform = {0, 0, 1, 0, 0, 0, 0, 0};
	auto print = [] (std::ostream &os, Model<T> const& b) -> std::ostream& {
		return os << "transform = " << b.transform
				<< ", accumulated = " << b.accumulated, os;
	};
	std::cout << "Before accumulation:" << std::endl;
	print(std::cout << "  Parent: ", parent) << std::endl;
	print(std::cout << "  Child: ", child) << std::endl;
	parent.accumulate(DQ{1});
	std::cout << "After:" << std::endl;
	print(std::cout << "  Parent: ", parent) << std::endl;
	print(std::cout << "  Child: ", child) << std::endl;

	leaf.add_vertices(1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1)
			.add_indices(0, 1, 2);

}
