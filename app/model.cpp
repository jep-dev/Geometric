#include <iostream>

#include "model.hpp"
#include "dual.tpp"
#include "streams.tpp"

template<class S, class T, class... I>
S& print_model(S &s, Model<T, I...> const& m, unsigned level = 0);
template<class S, class T, class... I>
S& print_structure(S &s, Model<T, I...> const& m);

template<class S, class T, class... I>
S& print_structure(S &s, Model<T, I...> const& m) {
	using namespace Streams;
	s << to_string(m.accumulated, 2) << " { ";
	for(auto const& c : m.children)
		print_structure(s, c);
	return s << " }", s;
}

template<class S, class T, class... I>
S& print_model(S &s, Model<T, I...> const& m, unsigned level) {
	using namespace Streams;
	s << string(level*2, ' ') << to_string(m.accumulated, 2) << "\n";
	for(auto & c : m.children)
		print_model(s, c, level+1);
	return s;
}

int main(int argc, const char *argv[]) {
	using namespace Streams;
	using std::cout;
	using std::endl;
	typedef float T;
	typedef DualQuaternion<T> DQ;

	Model<T> root;
	// This creates two nodes, but replaces the first with the second
	// auto lhs = &root.emplace_back(...);
	//      lhs = &lhs -> emplace_back(...);
	auto rhs = &root.emplace_back({1_e + 1_K});
	auto lhs = &root.emplace_back({1_e + 1_I});
	lhs = &lhs -> emplace_back({1_e + 10_I});
	rhs = &rhs -> emplace_back({1_e + 1_K});
	print_model(cout, root);
	root.accumulate(1_e + 1_I);
	print_model(cout, root);
}
