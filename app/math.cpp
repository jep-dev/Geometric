#include <iostream>
#include <iomanip>
#include <vector>

#include "math.hpp"
#include "parse.hpp"

using namespace Streams;

template<class S>
std::ostream& center(std::ostream& out, S const& s, unsigned N) {
	using namespace std;
	ostringstream oss;
	oss << s;
	auto str = oss.str();
	auto len = str.length(), dlen = N - len, dlen0 = dlen/2, dlen1 = dlen - dlen0;
	return out << setw(dlen0) << "" << str << setw(dlen1) << "";
}

void test_near(void) {
	using namespace std;
	typedef float T;

	static constexpr const char *epsilon = "\u03B5";
	//T nl = .0625f, dnl = sqrtf(2), nls[] = {nl, nl*dnl, nl*powf(dnl,2), nl*powf(dnl,3), nl*powf(dnl,4)};
	T nl = .0625f, dnl = nl, nls[] = {nl, nl+dnl, nl+dnl*2, nl+dnl*3, nl+dnl*4};

	static constexpr unsigned n_nls = sizeof(nls)/sizeof(nls[0]);
	unsigned M = 16, N = 24,
			nDigitsM = numDigits(M), nDigitsN = numDigits(N),
			nTotal = N*(nDigitsN+1)+nDigitsM+1;
	auto setM = setw(nDigitsM), setN = setw(nDigitsN),
			setT = setw(nTotal), setShortT = setw(nTotal - nDigitsM - 1);

	string shades[n_nls+1] = {"", "`", "'", "\"", "*", "#"};
	auto interp = [=] (auto && i, auto && j) -> pair<T,T> {
		// return {log(T(i)/N+1), T(j)/N}; // Log-linear scale
		return {T(i)/M, T(j)/N}; // Linear scale
	};
	auto xformat = [=] (auto &s, auto x, bool lhs = false)
		-> decltype(s) { return s << (lhs ? setN : setM) << x, s; };
	auto hrule = [=] (auto &s) -> decltype(s) {
		auto f = cout.fill();
		s << setfill(' ') << setM << "" << '+'
				<< setfill('-') << setShortT << "" << '+';
		return s << setfill(f), s;
	};

	hrule(cout) << endl;
	xformat(cout, "", true) << '|';
	for(unsigned j = 0; j < N; j++)
		xformat(cout, j) << ' ';
	hrule(cout << '|' << endl) << endl;
	for(unsigned i = 0; i < M; i++) {
		xformat(cout, i, true) << '|';
		for(unsigned j = 0; j < N; j++) {
			auto uv = interp(i, j);
			cout << right;
			unsigned hits = 0;
			for(auto const& lim : nls)
				if(near(uv.first, uv.second, lim)) hits++;
			if(hits) xformat(cout, shades[hits]) << ' ';
			else if(near_zero(uv.first, nl) || near_zero(uv.second, nl))
				xformat(cout, '0') << ' ';
			else xformat(cout, shades[0]) << ' ';
		}
		endl(cout << '|');
	}
	hrule(cout) << endl;
}


void print_split(std::vector<std::string> const& in) {
	bool met = false;
	for(auto const& s : in) {
		auto split = split_outer(s);
		if(met) std::cout << ", ";
		met = true;
		switch(split.size()) {
			case 0:
				break;
			case 1:
				std::cout << split[0];
				break;
			default:
				std::cout << '{';
				print_split(split);
				std::cout << '}';
				break;
		}
	}
}

void test_interactive(void) {
	std::cout << "Enter an expression: \n";

	std::string line;
	std::getline(std::cin, line);
	if(line == "") return;

	auto assigned = split_assign(line);
	auto len0 = assigned.first.length(),
			len1 = assigned.second.length();
	if(len0)
		std::cout << '[' << assigned.first << ']';
	if(len0 && len1)
		std::cout << " = ";
	if(len1) {
		std::cout << '`';
		print_split(split_outer(assigned.second));
		std::cout << '`';
	}
	std::cout << std::endl;
	test_interactive();
}

int main(int argc, const char *argv[]) {
	bool interactive = false;
	for(unsigned i = 1; i < argc; i++) {
		if(std::string(argv[i]) == "-i") interactive = true;
	}
	if(interactive) {
		test_interactive();
	} else {
		test_near();
	}
}
