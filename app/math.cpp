//#include <ctype>
#include <limits>

#include <iostream>
#include <iomanip>
#include <vector>
#include <set>

#include "math.hpp"
#include "parse.hpp"
#include "dual.hpp"
#include "dual.tpp"

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

std::vector<std::string> split_sum(std::string const& line) {
	std::vector<std::string> split;
	std::string cur;
	for(unsigned i = 0, n = line.length(); i < n; i++) {
		char c = line[i];
		switch(c) {
			case '+':
				if(cur != "") {
					split.emplace_back(cur);
					cur = "";
				}
				break;
			case '-':
				if(cur != "") {
					split.emplace_back(cur);
					cur = "";
				}
				cur += c;
				break;
			default:
				cur += c;
				break;
		}
	}
	if(cur != "") split.emplace_back(cur);
	return split;
}

// Note - a0b.12cd3ef is parsed as 0.123, etc.
// Actual number extraction is locale-defined, e.g. 3,142
// Grouping characters are not supported, e.g. 3 000 000
template<class T = float>
auto parse_coefficient(std::string const& word)
		-> std::enable_if_t<std::is_arithmetic<T>::value
			&& std::numeric_limits<T>::has_quiet_NaN, T> {
	T out;
	std::string value;
	for(auto c : word) {
		// TODO better criteria for value characters?
		if(std::isdigit(c) || std::ispunct(c))
			value += c;
	}
	if(value == "")
		return 1;
	std::istringstream iss(value);
	iss >> out;
	if(iss.fail())
		return std::numeric_limits<T>::quiet_NaN();
	return out;
}

// Same note as coefficient: a0b.12cd3ef is parsed as abcdef
// Skips digits, punctuation, and spaces explicitly; accepts printable
// e.g. unicode pi, epsilon, etc.
std::string parse_units(std::string const& word) {
	std::string units;
	for(auto c : word) {
		// if(std::isalpha(c)) units += c;
		if(std::isdigit(c) || std::ispunct(c) || std::isspace(c))
			continue;
		if(std::isprint(c))
			units += c;
	}
	return units;
}

template<class T>
std::pair<bool, DualQuaternion<T>> parse_dual(std::string const& word) {
	std::pair<bool, DualQuaternion<T>> out = { false, {1} };
	auto & d = out.second;

	T coefficient = parse_coefficient<T>(word);
	d *= coefficient;

	auto units = parse_units(word);
	static const DualQuaternion<T> Z = {0};
	bool p = false, latin = false, greek = false;
	char prev = '\0';
	for(auto c : units) {
		if(p) {
			p = false;
			// Accept PI/pi
			if((prev == 'p' && c == 'i') || (prev == 'P' && c == 'I')) {
				d *= T(M_PI);
				// Prevent parsing as I/i
				continue;
			}
			return out; // Reject P/p otherwise
		}
		if(latin) {
			latin = false;
			// Accept Latin epsilon, upper and lower
			if((prev == 0x01 && c == 0x90) || (prev == 0x02 && c == 0x5B)) {
				d *= 1_E;
				continue;
			}
			return out; // Reject Latin otherwise
		}
		if(greek) {
			greek = false;
			// Accept literal pi
			if(c == 0xC0) {
				d *= T(M_PI);
				continue;
			}
			// Accept Greek epsilon, upper and lower
			if(c == 0x95 || c == 0xB5) {
				d *= 1_E;
				continue;
			}
			return out; // Reject Greek otherwise
		}
		switch(c) {
			case 'e': break;
			case 'I': d *= 1_E;
			case 'i': d *= 1_i; break;
			case 'J': d *= 1_E;
			case 'j': d *= 1_j; break;
			case 'K': d *= 1_E;
			case 'k': d *= 1_k; break;
			case 'E': d *= 1_E; break;
			// Accept 'p' pending 'i' (as in pi)
			case 'P': case 'p': p = true; break;
			case 0x01: case 0x02: latin = true; break;
			case 0x03: greek = true; break;
			default: return out;
		}
		if(d == Z) return out.first = true, out;
		prev = c;
	}
	// Reject trailing 'p' and incomplete Latin/Greek
	if(p || latin || greek) return out;
	return out.first = true, out;
}

void test_interactive(void) {
	std::cout << "Enter an expression: \n";

	std::string line;
	std::getline(std::cin, line);
	if(line == "") return;

	auto split = split_sum(line);
	/*if(split.size()) {
		std::cout << "{";
		for(long n = split.size(), i = 0; i < n; i++) {
			auto const& word = split[i];
			if(i) std::cout << ", ";
			std::cout << word << " = " << parse_coefficient(word)
					<< " * " << parse_units(word);
		}
		std::cout << "}" << std::endl;
	}*/
	typedef double T;
	DualQuaternion<T> sum = {0};
	if(split.size()) {
		for(auto const& word : split) {
			auto dp = parse_dual<T>(word);
			if(dp.first) {
				sum += dp.second;
				std::cout << "Sum + (\"" << word << "\" = " << dp.second << ") = " << sum;
			} else {
				std::cout << "Could not parse \"" << word << '"';
			}
			std::cout << std::endl;
		}
	}

	/*auto assigned = split_assign(line);
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
	std::cout << std::endl;*/
	test_interactive();
}

int main(int argc, const char *argv[]) {
	for(unsigned i = 1; i < argc; i++) {
		std::string arg = argv[i];
		if(arg == "-i" || arg == "--interactive") test_interactive();
		else if(arg == "-n" || arg == "--near") test_near();
	}
}
