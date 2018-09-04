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

void print_split(std::vector<std::string> const& in) {
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


// TODO figure out how to parse UTF8, etc.
template<class T>
std::pair<bool, DualQuaternion<T>> parse_dual(std::string const& word) {
	std::pair<bool, DualQuaternion<T>> out = { false, {1} };
	if(!word.length()) return out;
	auto & d = out.second;

	T coefficient = parse_coefficient<T>(word);
	d *= coefficient;

	auto units = parse_units(word);
	static const DualQuaternion<T> Z = {0};
	bool p = false, latin = false, greek = false;
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
		/*if(latin) {
			latin = false;
			// Accept Latin epsilon, upper and lower
			if(((prev == 0x01) && (c == 0x90)) || ((prev == 0x02) && (c == 0x5B))) {
				d *= 1_E;
				prev = c;
				continue;
			}
			return out; // Reject Latin otherwise
		}
		if(greek) {
			greek = false;
			// Accept literal pi
			if(c == 0xC0) {
				d *= T(M_PI);
				prev = c;
				continue;
			}
			// Accept Greek epsilon, upper and lower
			if((c == 0x95) || (c == 0xB5)) {
				d *= 1_E;
				prev = c;
				continue;
			}
			return out; // Reject Greek otherwise
		}*/
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
			/*
			case 0x01: case 0x02: latin = true; break;
			case 0x03: greek = true; break;
			*/
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
}

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

template<class T>
void test_interactive(std::map<std::string, DualQuaternion<T>> &symbols) {
	std::cout << "Enter an assignment or expression: ";

	bool silenced = false;
	std::string line;
	std::getline(std::cin, line);
	if(line == "") return;

	/*
	auto split = split_sum(line);
	if(split.size()) {
		std::cout << "{";
		for(long n = split.size(), i = 0; i < n; i++) {
			auto const& word = split[i];
			if(i) std::cout << ", ";
			std::cout << word << " = " << parse_coefficient(word)
					<< " * " << parse_units(word);
		}
		std::cout << "}" << std::endl;
	}
	*/

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

	/*auto len0 = assigned.first.length(),
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
	test_interactive();
	*/

	test_interactive(symbols);
}

int main(int argc, const char *argv[]) {
	using namespace std;
	typedef float T;
	typedef DualQuaternion<T> DQ;
	map<string, DQ> symbols;
	test_interactive(symbols);
	cout << "At exit, symbols are:" << endl;
	for(auto const& s : symbols)
		cout << "\t" << s.first << " = " << s.second << endl;
}
