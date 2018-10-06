#include <limits>

#include <iostream>
#include <iomanip>
#include <map>
#include <vector>
#include <set>

#include "math.tpp"
#include "parse.hpp"
#include "streams.tpp"
#include "dual.tpp"

#include "utility.hpp"
#include "geometric.hpp"

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
template<class K, class V>
bool contains(std::map<K, V> const& m, K const& k)
	{ return m.find(k) != m.cend(); }

bool isvar(std::string const& line, bool trimmed = false) {
	auto s = line;
	if(!trimmed) s = Streams::trim(s);
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
	if(!trimmed) return isvar(Streams::trim(s), true);
	return isvar(s, true);
}


template<class T = float>
T parse_value(std::string const& word) {
	T out = make_NaN<T>();
	std::string value;
	bool neg = false, major = false, minor = false, pt = false;
	char prev = '\0';
	for(auto c : word) {
		// TODO better criteria for value characters?
		if(std::isdigit(c)) {
			// if(!major && c == '0') ??
			if(pt) minor = true;
			else major = true;
			value += c;
		} else if(c == '.') {
			if(pt) return out;
			pt = major = true;
			value += c;
		} else if(c == '-') {
			if(major) return out;
			neg = !neg;
		} else break;
	}
	if(pt && !minor || value == "")
		return make_NaN<T>();
	if(neg && value == "") return -1;
	std::istringstream iss(value);
	iss >> out;
	if(iss.fail())
		return make_NaN<T>();
	return neg ? -out : out;
}
template<class T = float>
std::pair<T, std::string> parse_coefficient(std::string const& word) {
	std::pair<T, std::string> out;
	out.first = parse_value(word);
	// Should this function participate in name validation?
	bool start = false;
	for(auto const& c : word)
		if(start || c == '(' || std::isalpha(c))
			start = true, out.second += c;
	return out;
}

/*
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

	T coefficient = parse_value<T>(word);
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
			//std::cout << word << " = " << parse_value(word)
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

std::vector<std::string> parse_add(std::string const& expr) {
	using std::string;
	std::vector<string> out;
	string cur;
	int parens = 0;
	for(auto const& c : expr) {
		switch(c) {
			case '+':
				if(parens) cur += "+";
				else if(cur.length())
					out.emplace_back(cur), cur = "";
				break;
			case '-':
				if(parens) cur += "-";
				else if(cur.length())
					out.emplace_back(cur), cur = "-";
				break;
			case '(':
				parens++;
				cur += "(";
				break;
			case ')':
				parens--;
				cur += ")";
				break;
			default:
				cur += c;
				break;
		}
	}
	if(cur.length())
		out.emplace_back(cur);
	return out;
}


std::vector<std::string> parse_mul(std::string const& expr) {
	using std::string;
	std::vector<string> out;
	string cur;
	int parens = 0;
	bool num = false, alpha = false;
	for(auto const& c : expr) {
		if(std::isdigit(c) || c == '.') {
			if(alpha && cur.length()) {
				out.emplace_back(cur);
				cur = "";
			}
			alpha = false;
			num = true;
			cur += c;
		} else if(std::isalpha(c) || c == '_') {
			if(num && cur.length()) {
				out.emplace_back(cur);
				cur = "";
			}
			alpha = true;
			num = false;
			cur += c;
		} else {
			switch(c) {
			case '(':
				if(parens == 0 && cur.length()) {
					out.emplace_back(cur);
					cur = "";
				}
				cur += '(';
				parens++;
				num = false;
				alpha = false;
				break;
			case ')':
				parens--;
				cur += ')';
				if(parens == 0) {
					out.emplace_back(cur);
					cur = "";
				}
				num = false;
				alpha = false;
				break;
			}
		}
	}
	if(cur.length()) out.emplace_back(cur);
	return out;
}

/*  Operators in parse order (reverse order of ops)
 *            A  S  M  U  P
 *  Addition     1  2  3  4
 *  Sandwich        1  2  3
 *  Mult.     1        2  3
 *  Unary              1  2
 *  Parens    1  2  3  4
 *  This places '^' (sandwich) much later in the oerder of ops, but informally,
 *  people write and read e^3pi/4 as 'e^(3pi/4)' and not '(e^3)pi/4'.
 */

template<class T, class C = std::map<std::string, T>, class V = C>
T eval_add(C & constants, V & variables, std::string const& expr) {
	auto terms = parse_add(expr);
	T out = {};
	if(!terms.size())
		return out;
	for(auto const& term : terms) {
		if(contains(constants, term)) out += constants[term];
		else if(contains(variables, term)) out += variables[term];
		else {
			T termval = {1};
			auto factors = parse_mul(term);
			for(auto const& factor : factors) {
				auto copair = parse_coefficient(factor);
				if(contains(constants, copair.second))
					termval *= copair.first * constants[copair.second];
				else if(contains(variables, copair.second))
					termval *= copair.second * variables[term];
				else {
					/* TODO iron out NaN handling (and var name equiv) */
					termval *= copair.first;
				}
			}
		}
	}
}

std::pair<std::string, std::string> parse_assign(std::string const& line) {
	auto s = Streams::trim(line);
	auto pos = s.find('=');
	if(pos == std::string::npos) return std::make_pair(s, "");
	return std::make_pair(Streams::trim(s.substr(0, pos)), Streams::trim(s.substr(pos+1)));
}

template<class T = float>
using Duals = std::map<std::string, DualQuaternion<T>>;

template<class T = float>
struct System {
	Duals<T> vars = {}, cvars = {};

	bool assign(std::string const& s) {
		if(!s.length()) return false;
		auto assign = parse_assign(s);
		std::string lhs = assign.first, rhs = assign.second;
		if(!lhs.length() || !rhs.length()) return false;
		bool lhs_isvar = isvar(lhs), rhs_isvar = isvar(rhs),
				lhs_iscdef = lhs_isvar && contains(cvars, lhs),
				rhs_iscdef = rhs_isvar && contains(cvars, rhs),
				lhs_isdef = lhs_isvar && contains(vars, lhs),
				rhs_isdef = rhs_isvar && contains(vars, rhs);
		if(lhs_iscdef) return false;
		if(rhs_iscdef) vars[lhs] = cvars[rhs];
		else if(rhs_isdef) vars[lhs] = vars[rhs];
		else { /* TODO Actually parse here */ }
		return true;
	}
};

int main(int argc, const char *argv[]) {
	using namespace std;
	typedef float T;
	typedef DualQuaternion<T> DQ;

	Duals<T> vars = {}, cvars = {
		{"pi", T(M_PI)*1_e}, {"PI", T(M_PI)*1_e},
		{"e", 1_e}, {"i", 1_i}, {"j", 1_j}, {"k", 1_k},
			{"ij", 1_k}, {"jk", 1_i}, {"ki", 1_j},
		{"E", 1_E}, {"I", 1_I}, {"J", 1_J}, {"K", 1_K},
			{"Ei", 1_I}, {"Ej", 1_J}, {"Ek", 1_K}
	};

	string line;
	while(getline(cin, line)) {
		if(!line.length()) break;
		auto assign = parse_assign(line);
		auto lhs = assign.first, rhs = assign.second;
		bool lisempty = !lhs.length(),
				lisvar = !lisempty && isvar(lhs),
				lvar = lisvar && contains(vars, lhs),
				lconst = lisvar && !lvar && contains(cvars, lhs),
			risempty = !rhs.length(),
				risvar = !risempty && isvar(rhs),
				rvar = risvar && contains(vars, rhs),
				rconst = risvar && contains(cvars, rhs);
		if(lisempty) {
			cout << "There was some kind of error parsing " << line << "." << endl;
			continue;
		}
		DQ out = {0};
		/*if(risempty) {
			rhs = lhs; // TODO something else?
		}*/
		//if(lisvar) {
			bool cont = true;
			if(risempty) {
				cout << lhs << " = ";
				if(lconst) cont = false, cout << cvars[lhs];
				else if(lvar) cout << vars[lhs];
				else rhs = lhs;
			}
			//if(!risempty) {
				if(cont && lconst) cout << lhs << " is constant" << endl;
				else if(rconst) vars[lhs] = cvars[rhs];
				else if(contains(vars, rhs)) vars[lhs] = vars[rhs];
				else {
					//cout << lhs << " = ";
					bool first_add = true;
					for(auto term : parse_add(rhs)) {
						if(!first_add) cout << " + ";
						first_add = false;
						//cout << '[';
						auto factors = parse_mul(term);
						if(factors.size() > 1) {
							cout << "*[";
							bool first_mul = true;
							for(auto const& f : factors) {
								if(!first_mul) cout << ", ";
								first_mul = false;
								cout << f;
							}
							cout << ']';
						} else if(factors.size() == 1) {
							cout << factors[0];
						} else {
							cout << "<?>";
						}
						//cout << ']';
					}
					cout << endl;

					/* TODO evaluate and set vars[lhs] */

					/*for(long i = 0, N = add.size(); i < N; i++) {
						if(i) cout << ", ";
						cout << add[i];
					}*/
					//cout << "]" << endl;
				}
				//cout << lhs << " = " << vars[lhs] << endl;
		//} else {
			//cout << '"' << lhs << "\" is not a valid variable name" << endl;
		//}
		/*if(risempty) {
			if(isvar(lhs)) {
				if(!contains(cvars, lhs)) {
					if(isvar(rhs)) {
						if(contains(cvars, rhs)) {
							vars[lhs] = cvars[rhs];
							cout << lhs << " := " << vars[lhs] << endl;
						} else if(contains(vars, rhs)) {
							vars[lhs] = vars[rhs];
							cout << lhs << " := " << vars[lhs] << endl;
						} else {
							cout << lhs << " := (evaluation of " << rhs << ")" << endl;
						}
					} else {
						cout << lhs << " := (evaluation of " << rhs << ")" << endl;
					}
				} else if(contains(cvars, lhs)) {
					cout << "Cannot assign " << lhs << " from " << cvars[lhs]
							<< " to (evaluation of " << rhs << ")" << endl;
				} else {
					if(isvar(rhs)) {
						cout << lhs << " := ";
						if(contains(cvars, rhs)) cout << cvars[rhs] << endl;
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
				if(contains(cvars, lhs)) cout << cvars[lhs] << endl;
				else if(contains(vars, lhs)) cout << vars[lhs] << endl;
				else cout << "(evaluation of " << lhs << ")" << endl;
			} else cout << "(evaluation of " << lhs << ")" << endl;
		}*/
	}

	/*map<string, DQ> symbols;
	test_interactive(symbols);
	cout << "At exit, symbols are:" << endl;
	for(auto const& s : symbols)
		cout << "\t" << s.first << " = " << s.second << endl;*/
}
