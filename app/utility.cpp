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
	typedef Value_t<false, T> VT_T;
	typedef Value_t<true, T> VTR_T;
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
	s << prettyTrunc<Value_t<false, T>>() << " and deep "
	return print_types(s, rhs, Value_t<false, T>(), Value_t<true, T>());*/
}
template<class S, class T, class U>
std::ostream& print_sumValueTypes(S &s, T const& t, U const& u) {
	using namespace Detail;
	typedef SumType_t<T, U> TU;
	typedef Value_t<false, T> VT_T;
	typedef Value_t<false, U> VT_U;
	typedef Value_t<false, TU> VT_TU;
	typedef Value_t<true, T> VTR_T;
	typedef Value_t<true, U> VTR_U;
	typedef Value_t<true, VT_TU> VTR_TU;
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
template<class T> using T2 = T1<T1<T>>;
//template<class T> struct T2 { typedef T1<T> value_type; };
// template<class T> struct T3 : T1<T> { };

/*template<class S, class T>
auto operator+(S const& lhs, T const& rhs)
	-> T3<T1<Detail::SumValue_t<true, S, T>>> { return {}; }*/

template<class T> using VAL = Detail::Value_t<false, T>;
template<class T> using VALR = Detail::Value_t<true, T>;
template<class S, class T> using ADD = Detail::SumType_t<S,T>;
template<class S, class T> using ADDR = Detail::SumValue_t<true, S, T>;
template<class S, class T> using SUB = Detail::DifferenceType_t<S,T>;
template<class S, class T> using SUBR = Detail::DifferenceValue_t<true, S,T>;
template<class S, class T> using MUL = Detail::ProductType_t<S,T>;
template<class S, class T> using MULR = Detail::ProductValue_t<true, S,T>;
template<class S, class T> using DIV = Detail::QuotientType_t<S,T>;
template<class S, class T> using DIVR = Detail::QuotientValue_t<true, S,T>;

template<class S, class T>
auto operator+(T1<S> const&, T1<T> const&)
		-> T1<ADD<S,T>> { return {}; }
template<class S, class T>
auto operator-(T1<S> const&, T1<T> const&)
		-> T1<SUB<S,T>> { return {}; }
template<class S, class T>
auto operator*(T1<S> const&, T1<T> const&)
		-> T1<MUL<S,T>> { return {}; }
template<class S, class T>
auto operator/(T1<S> const&, T1<T> const&)
		-> T1<DIV<S,T>> { return {}; }


int main(int argc, const char *argv[]) {
	using namespace Detail;
	using namespace std;


	typedef int I; typedef T1<I> I1; typedef T2<I> I2;
	typedef float F; typedef T1<F> F1; typedef T2<F> F2;

	static_assert(is_same<VAL<I2>, I1>::value && is_same<VAL<I1>, I>::value,
		"Value_t is the outermost value_type");
	static_assert(is_same<VALR<I1>, I>::value && is_same<VALR<I2>, I>::value,
		"Recursive Value_t is the innermost value_type");

	static_assert(is_same<ADD<I,F>,F>::value,
		"SumType_t is the sum of outermost value_types");
	static_assert(is_same<decltype(declval<I1>()+declval<F1>()), F1>::value,
		"T1<S> + T1<T> = T1<S+T>");
	static_assert(is_same<decltype(declval<I2>()+declval<F2>()), T1<F1>>::value,
		"T1<T1...<S>> + T1<T1...<T>> = T1<T1...<S+T>>");
		// = T1<T1...<S> + T1...<T>> = T1<T1...<S+T>>
	// T1...<S> + T1<T1...<T>> is only defined if S = T1<S'>
	static_assert(is_same<ADDR<I1,F1>, ADDR<I2,F2>>::value,
		"Recursive SumType_t is the sum of innermost value_types");

	static_assert(is_same<SUB<I,F>,F>::value,
		"DifferenceType_t is the sum of outermost value_types");
	static_assert(is_same<decltype(declval<I1>()-declval<F1>()), F1>::value,
		"T1<S> - T1<T> = T1<S-T>");
	static_assert(is_same<decltype(declval<I2>()-declval<F2>()), T1<F1>>::value,
		"T1<T1...<S>> - T1<T1...<T>> = T1<T1...<S-T>>");
	static_assert(is_same<SUBR<I1,F1>, SUBR<I2,F2>>::value,
		"Recursive DifferenceType_t is the sum of innermost value_types");

	static_assert(is_same<MUL<I,F>,F>::value,
		"ProductType_t is the sum of outermost value_types");
	static_assert(is_same<decltype(declval<I1>()*declval<F1>()), F1>::value,
		"T1<S> * T1<T> = T1<S*T>");
	static_assert(is_same<decltype(declval<I2>()*declval<F2>()), T1<F1>>::value,
		"T1<T1...<S>> * T1<T1...<T>> = T1<T1...<S*T>>");
	static_assert(is_same<MULR<I1,F1>, MULR<I2,F2>>::value,
		"Recursive ProductType_t is the sum of innermost value_types");

	static_assert(is_same<DIV<I,F>,F>::value,
		"QuotientType_t is the sum of outermost value_types");
	static_assert(is_same<decltype(declval<I1>()/declval<F1>()), F1>::value,
		"T1<S> / T1<T> = T1<S/T>");
	static_assert(is_same<decltype(declval<I2>()/declval<F2>()), T1<F1>>::value,
		"T1<T1...<S>> / T1<T1...<T>> = T1<T1...<S/T>>");
	static_assert(is_same<DIVR<I1,F1>, DIVR<I2,F2>>::value,
		"Recursive QuotientType_t is the sum of innermost value_types");


	print_sumValueTypes(cout, I1{}, F1{}) << endl;
	print_sumValueTypes(cout, I2{}, F2{}) << endl;
}
