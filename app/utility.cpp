#include "pretty.tpp"
#include "streams.tpp"
#include "utility.hpp"
#include <iostream>
#include <sstream>


template<class T> struct T1 { typedef T value_type; };
template<class T> struct T2 { typedef T1<T> value_type; };

template<class S, class T>
std::ostream& print_vt(S& s, T const& rhs) {
	s << std::string(Pretty<T>()) << " -> " << std::string(Pretty<typename T::value_type>())
			<< " -> " << std::string(Pretty<Detail::ValueType_t<T>>()) << std::endl;
	return s;
}
template<class S, class T, class U>
std::ostream& print_add_vt(S &s, T const& t, U const& u) {
	print_vt(s, t);
	print_vt(s, u);
	s << INDENT << std::string(Pretty<T>()) << " + " << std::string(Pretty<U>())
			<< " = " << std::string(Pretty<Detail::Add_ValueType_t<T,U>>()) << std::endl;
	return s;
}

int main(int argc, const char *argv[]) {
	using namespace Detail;
	using namespace std;

	typedef float F;
	typedef double D;
	typedef T1<F> F1; typedef ValueType_t<F1> VT_F1;
	typedef T2<F> F2; typedef ValueType_t<F2> VT_F2;
	typedef Add_ValueType_t<F1, F2> VT_F;
	static_assert(is_same<VT_F1, F>::value && is_same<VT_F2, F>::value,
			"ValueType_t must yield the innermost value_type");
	static_assert(is_same<VT_F, F>::value,
			"Add_ValueType_t must yield the sum type of innermost value_types");


	typedef T1<D> D1; typedef ValueType_t<D1> VT_D1;
	typedef T2<D> D2; typedef ValueType_t<D2> VT_D2;
	typedef Add_ValueType_t<D1, D2> VT_D;
	static_assert(is_same<VT_D1, D>::value && is_same<VT_D2, D>::value,
			"ValueType_t must yield the innermost value_type");
	typedef Add_ValueType_t<F1, F2, D1, D2> VT_FD;
	static_assert(is_same<VT_D, D>::value && is_same<VT_FD, D>::value,
			"Add_ValueType_t must yield the sum type of innermost value_types");
	print_add_vt(cout, F1{}, D1{});
	print_add_vt(cout, F2{}, D2{});
}
