#include "dual2.hpp"
#include "quaternion.tpp"
#include "streams.tpp"
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>
#include "pretty.tpp"

template<class S, char C, int POW>
S& operator<<(S &s, Dual2::Unit<C, POW>)
	{ return s << "{e" << C << '^' << POW << "}", s; }

int main(int argc, const char *argv[]) {
	static constexpr char ID = 'e', EPS = 'E', BETA = 'B';
	using namespace Dual2;
	using namespace std;

/*
template<class S, class DELIM = const char*>
std::enable_if_t<std::is_scalar<S>::value, std::string>
to_string(S s, unsigned prec = 0, bool show_zero = true, bool fill_zeroes = false)
*/
	typedef float T;
	unsigned prec = 3;
	bool show_zero = false, fill_zeroes = false;
	auto toStr = [=] (auto x) -> string
		{ return to_string(x, prec, show_zero, fill_zeroes); };

	T pi = M_PI;
	auto strPi = "(" + toStr(pi) + ")";
	typedef Quaternion<T> Q;

	std::ostringstream oss;
	Wrapper<T> wtDef;
	decltype(auto) tDef = unwrap(wtDef);
	oss << Pretty<decltype(wtDef)>() << " -> "
			<< Pretty<decltype(move(tDef))>() << " -> "
			<< toStr(tDef) << endl;
	// Dual2::Wrapper<float, void> -> float&& -> 0

	Wrapper<T> wtVal = wrap(pi);
	decltype(auto) tVal = unwrap(wtVal);
	oss << Pretty<decltype(wtVal)>() << ' ' << strPi << " -> "
			<< Pretty<decltype(move(tVal))>() << " -> "
			<< toStr(tVal) << endl;
	// Wrapper<float, void> -> float&& -> 3.141

	Wrapper<Q> wsDef;
	decltype(auto) sDef = unwrap(wsDef);
	oss << Pretty<decltype(move(wsDef))>() << " -> "
			<< Pretty<decltype(move(sDef))>() << " -> "
			<< toStr(sDef) << endl;
	// Wrapper<Quaternion<float>, void> -> Quaternion<float>&& -> 0

	Wrapper<Q> wsVal(T(M_PI));    // OK
	// Wrapper<Q> wsVal{T(M_PI)}; // OK
	decltype(auto) sVal = unwrap(wsVal);
	oss << Pretty<decltype(wsVal)>() << ' ' << strPi << " -> "
			<< Pretty<decltype(move(sVal))>() << " -> "
			<< toStr(sVal) << endl;
	// Wrapper<Quaternion<float>, void> -> Quaternion<float>&& -> 3.141e


	typedef Unit<> R;
	typedef Unit<'i'> Xi;
	typedef Unit<'j'> Yj;
	typedef Unit<'k'> Zk;
	typedef Unit<'E', 1> QE;

	oss << Pretty<Xi>() << ", " << Pretty<Yj>() << ", " << Pretty<Zk>() << "; " << endl;

	auto xy = units<-1, 1, 'i', 'j'>();
	auto z = Zk{};
	auto xyz = xy * z;
	oss << Pretty<decltype(xy)>() << " * " << Pretty<decltype(z)>() << "\n"
			"     = " << Pretty<decltype(xyz)>() << endl;

	auto str = oss.str();
	for(pair<string,string> strip :
			{make_pair("Dual2::", ""), {"Detail::", ""}, {"> >", ">>"}}) {
		auto pos = str.find(strip.first);
		while((pos = str.find(strip.first, pos)) != string::npos)
			str.replace(pos++, strip.first.length(), strip.second);
	}
	cout << str << endl;
}
