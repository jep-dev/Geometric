#include "pretty.tpp"
#include "streams.tpp"
#include "utility.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>


template<class S, class... T>
S& print_types(S& s) {
	for(std::string p : {prettyTrunc<T>()...})
		s << p << "; ";
	return s;
}
template<class S, class... T>
S& print_types(S& s, T &&... t) { return print_types<S, T...>(s); }

template<class S, class T>
S& print_valueTypes(S& s, T const& rhs) {
	using namespace Detail;
	using namespace std;
	s << INDENT << prettyTrunc<T>();
	typedef ValueType_t<false, T> VT_T;
	typedef ValueType_t<true, T> VTR_T;
	if(!is_same<T, VTR_T>::value) {
		s << ", contains ";
		if(!is_same<T, VT_T>::value) {
			s << "shallow ";
			if(!is_same<VT_T, VTR_T>::value) {
				s << prettyTrunc<VT_T>() << " ";
			}
			s << "and deep " << prettyTrunc<VTR_T>();
		} else {
			s << "deep " << prettyTrunc<VTR_T>();
		}
	}
	return s << std::endl, s;
	/*s << ", contains shallow ";
	s << prettyTrunc<ValueType_t<false, T>>() << " and deep "
	return print_types(s, rhs, ValueType_t<false, T>(), ValueType_t<true, T>());*/
}
template<class S, class T, class U>
std::ostream& print_sumValueTypes(S &s, T const& t, U const& u) {
	using namespace Detail;
	typedef SumType_t<T, U> TU;
	typedef ValueType_t<false, T> VT_T;
	typedef ValueType_t<false, U> VT_U;
	typedef ValueType_t<false, TU> VT_TU;
	typedef ValueType_t<true, T> VTR_T;
	typedef ValueType_t<true, U> VTR_U;
	typedef ValueType_t<true, VT_TU> VTR_TU;
	s << "Sums of types...\n";
	print_types<S, T, VT_T, VTR_T>(s << INDENT << "T: ") << "\n";
	print_types<S, U, VT_U, VTR_U>(s << INDENT << "T: ") << "\n";
	s << " Direct sum: " << prettyTrunc<T>() << " + " << prettyTrunc<U>()
			<< " = " << prettyTrunc<TU>() << " (contains " << prettyTrunc<VT_TU>() << ")\n"
		"Shallow sum: " << prettyTrunc<VT_T>() << " + " << prettyTrunc<VT_U>()
			<< " = " << prettyTrunc<VT_TU>() << " (contains " << prettyTrunc<VTR_TU>() << ")\n"
		"   Deep sum: " << prettyTrunc<VTR_T>() << " + " << prettyTrunc<VTR_U>()
				<< " = " << prettyTrunc<VTR_TU>() << " (no deeper value_type)\n";
	return s;
}

template<class T> struct T1 { typedef T value_type; };
template<class T> struct T2 { typedef T1<T> value_type; };
template<class T> struct T3 : T1<T> { };

template<class S, class T>
auto operator+(S const& lhs, T const& rhs)
	-> T3<T1<Detail::SumValueType_t<true, S, T>>> { return {}; }

int main(int argc, const char *argv[]) {
	using namespace Detail;
	using namespace std;

	typedef float F;
	typedef double D;
	typedef T1<F> F1; typedef T2<F> F2;
	typedef ValueType_t<false, F1> VT_F1;
	typedef ValueType_t<false, F2> VT_F2;
	typedef ValueType_t<true, F1> VTR_F1;
	typedef ValueType_t<true, F2> VTR_F2;
	typedef SumValueType_t<true, F1, F2> VTR_F;
	static_assert(is_same<VT_F2, F1>::value,
			"Without recursion, ValueType_t must yield the outermost value_type");
	static_assert(is_same<VTR_F1, F>::value && is_same<VTR_F2, F>::value,
			"With recursion, ValueType_t must yield the innermost value_type");
	static_assert(is_same<VTR_F, F>::value,
			"With recursion, SumValueType_t must yield the sum type of innermost value_types");


	typedef T1<D> D1;
	typedef T2<D> D2;
	/*typedef ValueType_t<false, D1> VT_D1;
	typedef ValueType_t<false, D2> VT_D2;
	typedef ValueType_t<true, D1> VTR_D1;
	typedef ValueType_t<true, D2> VTR_D2;
	//typedef SumValueType_t<false, D1, D2> VT_D;
	typedef SumValueType_t<true, D1, D2> VTR_D;
	static_assert(is_same<VT_D1, D>::value && is_same<VT_D2, D>::value,
			"Without recursion, SumValueType_t must yield the outermost value_type");
	static_assert(is_same<VTR_D1, D>::value && is_same<VTR_D2, D>::value,
			"With recursion, SumValueType_t must yield the innermost value_type");
	typedef SumValueType_t<F1, F2, D1, D2> VT_FD;
	static_assert(is_same<VT_D, D>::value && is_same<VT_FD, D>::value,
			"With recursion, SumValueType_t must yield the sum type of innermost value_types");*/
	print_sumValueTypes(cout, F1{}, D1{}) << endl;
	print_sumValueTypes(cout, F2{}, D2{}) << endl;
}
