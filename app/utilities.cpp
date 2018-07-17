#include <array>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

#include "utilities.hpp"
#include "pretty.hpp"

struct u{};
struct v{};
struct w{};
struct x{};
struct y{};
struct z{};

int main(int argc, const char *argv[]) {
	using std::cout;
	using std::endl;
	using std::pair;
	using std::string;
	using std::array;
	using std::vector;
	using namespace Detail;

	using E = Tag<>;
	using U = Tag<u>;
	using V = Tag<v>;
	using W = Tag<w>;
	using X = Tag<x>;
	using Y = Tag<y>;
	using Z = Tag<z>;

	std::ostringstream oss;
	oss << "Basic types:\n\t"
			"0 = " << make_pretty(E{}) << ";\n\t"
			"U = " << make_pretty(U{}) << "; "
			"V = " << make_pretty(V{}) << "; "
			"W = " << make_pretty(W{}) << ";\n\t"
			"X = " << make_pretty(X{}) << "; "
			"Y = " << make_pretty(Y{}) << "; "
			"Z = " << make_pretty(Z{}) << ".\n"
		"Direct sums:\n\t"
			"U+0 = " << make_pretty(U{}+E{}) << "; 0+X = " << make_pretty(E{}+X{}) << ";\n\t"
			"(U+V)+W = U+(V+W) = " << make_pretty((U{}+V{})+W{}) << " (associative.)\n\t"
			"U+V = " << make_pretty(U{}+V{})
				<< " != V+U = " << make_pretty(V{}+U{}) << " (non-commutative!)\n"
		"Inner product:\n\t"
			"0*0 = " << make_pretty(E{}*E{}) << ";\n\t"
			"U*X = " << make_pretty(U{}*X{}) << ";\n\t"
			"(U+V) * (X+Y) = " << make_pretty(U{}+V{}) << " * " << make_pretty(X{}+Y{})
				<< " = " << make_pretty((U{}+V{})*(X{}+Y{})) << "\n"
		"Outer products:\n\t"
			"0 ^ X = " << make_pretty(E{}^X{}) << ";\n\t"
			"U ^ 0 = " << make_pretty(U{}^E{}) << ";\n\t"
			"(U+V) ^ X = " << make_pretty((U{}+V{}) ^ X{}) << ";\n\t"
			"U ^ (X+Y) = " << make_pretty(U{} ^ (X{}+Y{})) << ";\n\t"
			"(U+V) ^ (X+Y) = " << make_pretty((U{}+V{}) ^ (X{}+Y{})) << ".\n\t"
			"(U+V+W) ^ (X+Y) = " << make_pretty((U{}+V{}+W{}) ^ (X{}+Y{})) << ".\n\t"
			"(U+V) ^ (X+Y+Z) = " << make_pretty((U{}+V{}) ^ (X{}+Y{}+Z{})) << ".\n\t"
			"(U+V+W) ^ (X+Y+Z) = " << make_pretty((U{}+V{}+W{}) ^ (X{}+Y{}+Z{})) << ".";
		/* I believe from a category/set/type theory standpoint, the orders of (U+V)^X and U^(X+Y)
		 * are one shy of what they should be. If (U+V)^(X+Y) is {{{u,x},{u,y}}, {{v,x},{v,y}}},
		 * then why wouldn't U^(X+Y) just be the same with the second element eliminated?
		 * Instead, it appears to *be* the first element (not the set containing it.) */
	auto res = oss.str();

	using S2 = array<string, 2>;
	for(auto const& it : vector<S2>{{"Detail::", ""},
			{"Tag", ""}, {"> >", ">>"}, {"<", "{"}, {">", "}"}}) {
		std::size_t pos;
		while((pos = res.find(it[0])) != std::string::npos)
			res.replace(pos, it[0].length(), it[1]);
	}
	cout << res << endl;
}
