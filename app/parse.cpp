#include <limits>

#include <iostream>
#include <iomanip>
#include <map>
#include <vector>
#include <set>

#include "math.hpp"
#include "parse.hpp"
#include "streams.tpp"
#include "dual.hpp"
#include "dual.tpp"

#include "utility.hpp"
#include "geometric.hpp"

#ifndef INDENT
#define INDENT "\t"
#endif

/*void print_split(std::vector<std::string> const& in) {
	bool met = false;
	for(auto const& s : in) {
		auto split = Streams::split_outer(s);
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
}*/

template<class T>
constexpr bool has_quiet_NaN(void)
	{ return std::is_arithmetic<T>::value && std::numeric_limits<T>::has_quiet_NaN; }
template<class T>
using QuietNaN = std::enable_if_t<has_quiet_NaN<T>(),
	std::integral_constant<T, std::numeric_limits<T>::quiet_NaN()>>;

template<class T>
constexpr bool has_loud_NaN(void)
	{ return std::is_arithmetic<T>::value && std::numeric_limits<T>::has_signaling_NaN; }
template<class T>
using LoudNaN = std::enable_if<has_loud_NaN<T>(),
	std::integral_constant<T, std::numeric_limits<T>::signaling_NaN()>>;

template<class S>
std::enable_if<has_quiet_NaN<S>(),S>
make_NaN(Detail::Tag<QuietNaN<S>> = {})
	{ return QuietNaN<S>::value; }
template<class S>
std::enable_if<has_loud_NaN<S>() && !has_quiet_NaN<S>(),S>
make_NaN(Detail::Tag<LoudNaN<S>> = {}) {
	return LoudNaN<S>::value;
}


// Note - a0b.12cd3ef is parsed as 0.123, etc.
// Actual number extraction is locale-defined, e.g. 3,142
// Grouping characters are not supported, e.g. 3 000 000
template<class T = float>
/*auto parse_coefficient(std::string const& word)
		-> std::enable_if_t<std::is_arithmetic<T>::value
			&& std::numeric_limits<T>::has_quiet_NaN, T> {*/
std::enable_if_t<has_quiet_NaN<T>(), T>
parse_coefficient(std::string const& word) {
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
		return QuietNaN<T>::value;
		//return std::numeric_limits<T>::quiet_NaN();
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


// TODO figure out how to parse UTF8, etc.
/*template<class T>
std::pair<bool, DualQuaternion<T>> parse_dual(std::string const& word) {
	std::pair<bool, DualQuaternion<T>> out = { false, {1} };
	if(!word.length()) return out;
	auto & d = out.second;

	T coefficient = parse_coefficient<T>(word);
	d *= coefficient;

	auto units = parse_units(word);
	static const DualQuaternion<T> Z = {0};
	bool p = false;
	char prev = '\0';
	for(auto const& c : units) {
		if(p) {
			p = false;
			// Accept PI/pi
			if(((prev == 'p') && (c == 'i')) || ((prev == 'P') && (c == 'I'))) {
				d *= T(M_PI);
				prev = c;
				// Prevent parsing as I/i
				continue;
			}
			return out; // Reject P/p otherwise
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
			default: return out;
		}
		if(d == Z) return out.first = true, out;
		prev = c;
	}
	// Reject trailing 'p'
	if(p) return out;
	// Reject incomplete Latin/Greek
	// if(latin || greek) return out;
	return out.first = true, out;
}
template<class T>
std::pair<bool, DualQuaternion<T>> parse_dual(std::vector<std::string> const& words) {
	if(!words.size()) return {false, {}};
	bool valid = true;
	DualQuaternion<T> out = {0};
	for(auto const& word : words) {
		auto parsed = parse_dual<T>(word);
		if(!parsed.first) {
			valid = false;
			break;
		}
		out += parsed.second;
	}
	return {valid, out};
}*/

template<class S>
S& help_dual(S& s) {
	return s << "Dual quaternions can be parsed from [-]au [{+|-}bv [...]], where\n"
		"\ta,b,... are {0..9}*[.{0..9}*] (any numeric literal) or pi;\n"
		"\tu,v,... are units in {e|i|j|k|E|I=Ei|J=Ej|K=Ek}; and\n"
		"\teach term is non-empty.\n", s;
}
template<class S>
S& help_assign(S& s) {
	return s << "Assignments must be given in the form <variable> = <expression>, where\n"
		"\t<variable> is a sequence of letters and not a sequence of {e|i|j|k|E|I|J|K|pi}, and\n"
		"\t<expression> is a sum of dual quaternion terms.\n", s;
}
template<class S>
S& help(S& s) {
	return s << "Inputs may be assignments (<variable> = <expression>) "
			"or free (<expression>).\n", s;
}

/*template<class T>
void test_interactive(std::map<std::string, DualQuaternion<T>> &symbols) {
	std::cout << "Enter an assignment or expression: ";

	bool silenced = false;
	std::string line;
	std::getline(std::cin, line);
	if(line == "") return;

	//auto split = split_sum(line);
	//if(split.size()) {
		//std::cout << "{";
		//for(long n = split.size(), i = 0; i < n; i++) {
			//auto const& word = split[i];
			//if(i) std::cout << ", ";
			//std::cout << word << " = " << parse_coefficient(word)
					//<< " * " << parse_units(word);
		//}
		//std::cout << "}" << std::endl;
	//}

	do {
		auto assigned = Streams::split_assign(line);
		auto & symbol = assigned.first, value = assigned.second;
		auto symbol_alpha = parse_units(symbol);
		auto symbol_dual = parse_dual<T>(symbol_alpha);
		auto value_dual = parse_dual<T>(split_sum(value));

		if((symbol.length() && !symbol_alpha.length()) || symbol_dual.first) {
			help_assign(std::cout);
			break;
		}
		if(!value.length() || !value_dual.first) {
			help_dual(std::cout);
			break;
		}
		if(symbol_alpha.length()) {
			std::cout << symbol_alpha << " = ";
			if(symbols.find(symbol_alpha) != symbols.end())
				std::cout << symbols[symbol_alpha] << " -> ";
			symbols[symbol_alpha] = value_dual.second;
		}
		std::cout << value_dual.second << std::endl;
	} while(0);

	//auto len0 = assigned.first.length(),
			//len1 = assigned.second.length();
	//if(len0)
		//std::cout << '[' << assigned.first << ']';
	//if(len0 && len1)
		//std::cout << " = ";
	//if(len1) {
		//std::cout << '`';
		//print_split(split_outer(assigned.second));
		//std::cout << '`';
	//}
	//test_interactive();

	test_interactive(symbols);
}*/

std::string trim(std::string const& s, bool leading = true, bool trailing = true) {
	long size = s.length(), front = 0, back = size-1;
	if(size == 0) return "";

	auto match = [] (char c) -> bool {
		switch(c) {
			case ' ': case '\t':
			case '\r': case '\n': return false;
			default: return true;
		}
	};

	if(leading) for(front = 0; front <= back; front++)
		if(match(s[front])) break;
	if(trailing) for(back = size-1; back >= front; back--)
		if(match(s[back])) break;

	return s.substr(front, back-front+1);
}

std::pair<std::string, std::string> parse_assign(std::string const& line) {
	auto s = trim(line);
	auto pos = s.find('=');
	if(pos == std::string::npos) return {s, ""};
	return { trim(s.substr(0, pos)), trim(s.substr(pos+1)) };
}

template<class K, class V>
bool contains(std::map<K, V> const& m, K const& k)
	{ return m.find(k) != m.cend(); }

bool isvar(std::string const& line, bool trimmed = false) {
	auto s = line;
	if(!trimmed) s = trim(s);
	if(!s.length() || !std::isalpha(s[0])) return false;
	for(auto const& c : s) {
		if(c == '_') continue;
		if(std::isalnum(c)) continue;
		return false;
	}
	return true;
}
bool isvar(std::string::const_iterator beg, std::string::const_iterator end,
		bool trimmed = false) {
	std::string s;
	for(auto cur = beg; cur < end; cur++) {
		s += *cur;
	}
	if(!trimmed) return isvar(trim(s), true);
	return isvar(s, true);
}

int main(int argc, const char *argv[]) {
	using namespace std;
	typedef float T;
	typedef DualQuaternion<T> DQ;

	map<string, T> scalars = {
		{"pi", M_PI}, {"PI", M_PI}
	};
	map<string, DQ> cduals = {
		{"e", 1_e}, {"i", 1_i}, {"j", 1_j}, {"k", 1_k},
			{"ij", 1_k}, {"jk", 1_i}, {"ki", 1_j},
		{"E", 1_E}, {"I", 1_I}, {"J", 1_J}, {"K", 1_K},
			{"Ei", 1_I}, {"Ej", 1_J}, {"Ek", 1_K}
	}, duals = {};

	string line;
	while(getline(cin, line)) {
		if(!line.length()) break;
		auto assign = parse_assign(line);
		auto const& lhs = assign.first, rhs = assign.second;
		if(rhs.length()) {
			if(isvar(lhs)) {
				if(!contains(cduals, lhs) && !contains(scalars, lhs)) {
					if(isvar(rhs)) {
						if(contains(cduals, rhs)) {
							duals[lhs] = cduals[rhs];
							cout << lhs << " := " << duals[lhs] << endl;
						} else if(contains(duals, rhs)) {
							duals[lhs] = duals[rhs];
							cout << lhs << " := " << duals[lhs] << endl;
						} else if(contains(scalars, rhs)) {
							duals[lhs] = DQ{scalars[rhs]};
							cout << lhs << " := " << duals[lhs];
						} else {
							cout << lhs << " := (evaluation of " << rhs << ")" << endl;
						}
					} else {
						cout << lhs << " := (evaluation of " << rhs << ")" << endl;
					}
				} else if(contains(scalars, lhs)) {
					cout << "Cannot assign " << lhs << " from " << scalars[lhs]
							<< " to (evaluation of " << rhs << ")" << endl;
				} else if(contains(cduals, lhs)) {
					cout << "Cannot assign " << lhs << " from " << cduals[lhs]
							<< " to (evaluation of " << rhs << ")" << endl;
				} else {
					if(isvar(rhs)) {
						cout << lhs << " := ";
						if(contains(scalars, rhs)) cout << scalars[rhs] << endl;
						else if(contains(cduals, rhs)) cout << cduals[rhs] << endl;
						else cout << "(evaluation of " << rhs << ")" << endl;
					} else {
						cout << lhs << " := (evaluation of " << rhs << ")" << endl;
					}
				}
			} else {
				cout << "The left hand side of assignment must be a variable name" << endl;
			}
		} else {
			if(isvar(lhs)) {
				cout << lhs << " = ";
				if(contains(scalars, lhs)) cout << scalars[lhs];
				else if(contains(cduals, lhs)) cout << cduals[lhs];
				else if(contains(duals, lhs)) cout << duals[lhs];
				else cout << "(unknown)";
				cout << endl;
			} else {
				cout << "(evaluation of " << lhs << ")" << endl;
			}
		}
	}

	/*map<string, DQ> symbols;
	test_interactive(symbols);
	cout << "At exit, symbols are:" << endl;
	for(auto const& s : symbols)
		cout << "\t" << s.first << " = " << s.second << endl;*/
}
