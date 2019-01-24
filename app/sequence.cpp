#include <iostream>
//#include <iomanip>
#include <sstream>

#include "binomial.hpp"
#include "dual.hpp"
#include "pretty.hpp"
#include "tag.hpp"

using namespace Detail;

template<class S, class T, T T0, T... TN>
S& operator<<(S& os, Seq<T, T0, TN...> const&)
	{ return os << T0, os; }
template<class S, class T, T T0, T T1, T... TN>
S& operator<<(S& os, Seq<T, T0, T1, TN...> const&)
	{ return os << T0 << ", " << Seq<T, T1, TN...>{}, os; }

template<class S, class T, T V>
S& operator<<(S& os, std::integral_constant<T, V>) {
	return os << "[T=" << to_string(V) << "]", os;
}

template<class T, T... TN>
std::string to_string(Seq<T, TN...> const& seq) {
	std::ostringstream oss;
	oss << seq;
	return "{" + oss.str() + "}";
}

template<class T, std::size_t N>
std::string substitute(std::string const& str, const T (&t) [N][2]) {
	std::string out = str;
	//for(auto const& p : {t...}) {
	for(auto const& p : t) {
		// auto pos = out.find(p.first);
		// while((pos = out.find(p.first)) != std::string::npos)
			// out.replace(pos++, p.first.length(), p.second);
		auto pos = out.find(p[0]);
		while((pos = out.find(p[0])) != std::string::npos)
			out.replace(pos++, p[0].length(), p[1]);
	}
	return out;
}

template<class... S, class... T>
std::string to_string(Tag<S...>, T &&... t) {
	std::string out, strs[] = {Pretty<S>()...}, pre = "{", post = "}";
	for(auto const& str : strs)
		out += pre + str, pre = ", ";
	out += post;
	return substitute(out, std::forward<T>(t)...);
}


template<class S>
S& testBinomial(S &s) { return s; }

template<class S, Size N, Size... I>
S& testBinomial(S &os, SeqSz<N, I...> = {}) {
	os << N << ". ";
	endl(os << Binomial<N>());
	return testBinomial<S, I...>(os);
}

template<class S, S V>
struct Integral: std::integral_constant<S, V> {
	using value_type = S;
	S value = V;
	constexpr operator S(void) const { return V; }
	constexpr S operator()(void) const { return V; }
	constexpr Integral(void): value(V) {}
};

int main(int argc, const char *argv[]) {
	using namespace std;
	// The executable size is independent of N, which should mean that the sequences
	// all disappear in abstract elimination.
	static constexpr unsigned N = 10, I = 4;
	typedef RepeatSeq<N, unsigned, 1> ones_t;                 // {1, 1, ..., 1, 1}
	typedef SumSeq<Value_t<ones_t>> sum_t;         // 10
	typedef PartialSumSeq<ones_t> inc_t;                      // {1, 2, ..., 9, 10}
	typedef ProductSeq<Value_t<inc_t>> product_t;  // 3628800
	typedef PartialSumSeq<Value_t<inc_t>> inc2_t;  // {1, 3, ..., 45, 55}
	std::string subs[][2] = {{"Detail::", ""}, {"std::", ""}};
	// noDetail[] = {"Detail::", ""}, noStd[] = {"std::", ""};
	// auto toStr = [=] (auto a) { return to_string(a, noDetail, noStd); };

	typedef Tag<int, char, int, char> tag_t;
	static constexpr std::size_t find0 = Find<char, tag_t>::value,
			find1 = Find<char, tag_t, find0+1>::value;
	ostringstream oss;
	oss << "Type sequence with repetition: "
			// << toStr(tag_t{}) << ":\n" // {int, char, int, char}:
			<< to_string(tag_t{}, subs) << ":\n" // {int, char, int, char}:
		"  - For Nth=" << Pretty<char>() << ", N=0, "
			"N' = " << find0 << "\n" // - For Nth=char, N=0, N' = 1
		"  - For Nth=" << Pretty<char>() << ", N=" << find0 << ", "
			"N' = " << find1 << "\n\n"; // - For Nth=char, N=1, N' = 3

	oss << "10 1's: " << to_string(ones_t{}) << "\n"
		"  - Sum: " << to_string(sum_t{}) << "\n"
	"  - Partial sum: " << to_string(inc_t{}) << "\n"
	"    - Evens: " << to_string(inc_t{}+inc_t{}) << "\n"
	"    - Product: " << to_string(product_t{}) << "\n"
	"    - Partial sum: " << to_string(inc2_t{}) << "\n"
	"    - Nth for N=" << I << ": " << get<I>(inc2_t{}) << "\n\n";

	oss << "Decompose<91> = " << decompose<int, 91, 7>() << '\n' << endl;

	oss << "Fibonacci<10> = " << Fibonacci<10>::value << '\n' << endl;

	typedef Integral<int, 5> ce_t;
	typedef std::integral_constant<int, ce_t{}.value> cei_t; // or...
	// typedef std::integral_constant<int, ce_t{}()> cei_t; // or...
	// typedef Integral<int, ce_t{}()> cei_t; // etc.

	oss << Pretty<ce_t>() << ": " << Pretty<cei_t>() << "\n\n";
	// This is just an example of constexpr 'members' - save for later


	oss << "Binomial coefficients: " << endl;
	testBinomial<ostream&, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9>(oss) << endl;

	cout << substitute(oss.str(), subs) << endl;

}
