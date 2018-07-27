#include <array>
#include <iostream>
#include <sstream>
#include <vector>

#include "utilities.hpp"
#include "pretty.tpp"

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
	using UV = Tag<u, v>;
	using W = Tag<w>;
	using VW = Tag<v, w>;
	using UVW = Tag<u, v, w>;
	using X = Tag<x>;
	using Y = Tag<y>;
	using XY = Tag<x, y>;
	using Z = Tag<z>;
	using YZ = Tag<y, z>;
	using XYZ = Tag<x, y, z>;

	static_assert(std::is_same<UV, decltype(U{}+V{})>::value
			&& std::is_same<VW, decltype(V{}+W{})>::value
			&& std::is_same<UVW, decltype(U{}+V{}+W{})>::value
			&& std::is_same<XY, decltype(X{}+Y{})>::value
			&& std::is_same<YZ, decltype(Y{}+Z{})>::value
			&& std::is_same<XYZ, decltype(X{}+Y{}+Z{})>::value,
		"Operator + must be direct sum of types (treated as orthogonal)");
	static_assert(std::is_same<decltype(UV{}+W{}), decltype(U{}+VW{})>::value
			&& std::is_same<decltype(XY{}+Z{}), decltype(X{}+YZ{})>::value,
		"Operator + must be associative");

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
			"U ^ X = " << make_pretty(U{}^X{}) << ";\n\t"
			"(U+V) ^ X = " << make_pretty((U{}+V{}) ^ X{}) << ";\n\t"
			"U ^ (X+Y) = " << make_pretty(U{} ^ (X{}+Y{})) << ";\n\t"
			"(U+V) ^ (X+Y) = " << make_pretty((U{}+V{}) ^ (X{}+Y{})) << ".\n\t"
			"(U+V+W) ^ (X+Y) = " << make_pretty((U{}+V{}+W{}) ^ (X{}+Y{})) << ".\n\t"
			"(U+V) ^ (X+Y+Z) = " << make_pretty((U{}+V{}) ^ (X{}+Y{}+Z{})) << ".\n\t"
			"(U+V+W) ^ (X+Y+Z) = " << make_pretty((U{}+V{}+W{}) ^ (X{}+Y{}+Z{})) << ".\n"
		"Evaluations:\n\t"
			"[0] = " << make_pretty(eval(E{})) << ";\n\t"
			"[U] = " << make_pretty(eval(U{})) << ";\n\t"
			"[U+V] = " << make_pretty(eval(U{}+V{})) << ";\n\t"
			"[(U+V)^(X+Y)] = " << make_pretty(eval((U{}+V{})^(X{}+Y{}))) << ".\n"
		"Access:\n\t"
			"[U+V]_0 = " << make_pretty(access<0>(eval(U{}+V{}))) << ";\n\t"
			"[U+V]_1 = " << make_pretty(access<1>(eval(U{}+V{}))) << ";\n\t"
			"[(U+V)^(X+Y)]_1 = " << make_pretty(access<1>(eval((U{}+V{})^(X{}+Y{})))) << ";\n\t"
			"[(U+V)^(X+Y)]_1_1 = " << make_pretty(access<1,1>(eval((U{}+V{})^(X{}+Y{})))) << ".";
	auto res = oss.str();

	using S2 = array<string, 2>;
	for(auto const& it : vector<S2>{{"Detail::", ""}, {"std::tuple", "Val"},
			{"Tag", ""}, {"> >", ">>"}, {"<", "{"}, {">", "}"}}) {
		std::size_t pos;
		while((pos = res.find(it[0])) != std::string::npos)
			res.replace(pos, it[0].length(), it[1]);
	}
	cout << res << endl;
}
