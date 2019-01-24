#include <array>
#include <iostream>
#include <sstream>
#include <vector>

#include "utility.hpp"
#include "sequence.hpp"
#include "tag.hpp"
#include "pretty.hpp"

template<class CRTP>
struct Base {
	int value = 0;
	template<class T>
	CRTP& operator=(T && t) {
		value = std::forward<T>(t);
		return static_cast<CRTP&>(*this);
	}
};

struct u; struct u : Base<u> { using Base::operator=; };
struct v; struct v : Base<v> { using Base::operator=; };
struct w; struct w : Base<w> { using Base::operator=; };
struct x; struct x : Base<x> { using Base::operator=; };
struct y; struct y : Base<y> { using Base::operator=; };
struct z; struct z : Base<z> { using Base::operator=; };


template<class S, class T>
S& operator<<(S &s, Base<T> const& b) {
	return s << Pretty<T>() << '(' << b.value << ')';
}

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
		using UV = Tag<u, v>;
		using VW = Tag<v, w>;
		using UVW = Tag<u, v, w>;
	using X = Tag<x>;
	using Y = Tag<y>;
	using Z = Tag<z>;
		using XY = Tag<x, y>;
		using YZ = Tag<y, z>;
		using XYZ = Tag<x, y, z>;
	using F = Tag<float>;
	using D = Tag<double>;

	string indent = "  ", para = "\n" + indent;

	std::ostringstream oss;
	oss << "Basic types: " << para
			<< "0 = " << pretty(E{}) << ";" << para
			<< "U = " << pretty(U{}) << "; "
			<< "V = " << pretty(V{}) << "; "
			<< "W = " << pretty(W{}) << ";" << para
			<< "X = " << pretty(X{}) << "; "
			<< "Y = " << pretty(Y{}) << "; "
			<< "Z = " << pretty(Z{}) << ".\n";

	static_assert(std::is_same<decltype(E{}+E{}), E>::value
			&& std::is_same<decltype(E{}+X{}), X>::value
			&& std::is_same<decltype(U{}+E{}), U>::value,
		"The empty tag must be the additive identity");
	oss << "Direct sums (operator+):" << para
			<< "0+0 = " << pretty(E{}+E{}) << "; "
			<< "U+0 = " << pretty(U{}+E{}) << "; "
			<< "0+X = " << pretty(E{}+X{}) << ";" << para;

	static_assert(std::is_same<decltype(UV{}+W{}), decltype(U{}+VW{})>::value
			&& std::is_same<decltype(XY{}+Z{}), decltype(X{}+YZ{})>::value,
		"Operator+ must be associative");
	oss << "(U+V)+W = U+(V+W) = " << pretty((U{}+V{})+W{}) << " (associative.)" << para;

	static_assert(std::is_same<decltype(V{}+U{}), decltype(reverse(UV{}))>::value,
		"Operator+ must preserve order (non-commutative)");
	oss << "U+V = " << pretty(U{}+V{})
				<< " != V+U = " << pretty(V{}+U{}) << " (non-commutative!)\n";

	static_assert(std::is_same<UV, decltype(U{}+V{})>::value
			&& std::is_same<VW, decltype(V{}+W{})>::value
			&& std::is_same<UVW, decltype(U{}+V{}+W{})>::value
			&& std::is_same<XY, decltype(X{}+Y{})>::value
			&& std::is_same<YZ, decltype(Y{}+Z{})>::value
			&& std::is_same<XYZ, decltype(X{}+Y{}+Z{})>::value,
		"Operator+ must be direct sum of types (treated as orthogonal)");
	oss << "Inner product (operator*):" << para
			<< "0*0 = " << pretty(E{}*E{}) << ";" << para
			<< "U*X = " << pretty(U{}*X{}) << ";" << para
			<< "(U+V) * (X+Y) = " << pretty(UV{}) << " * " << pretty(XY{})
				<< " = " << pretty(UV{}*XY{}) << ";" << para;
	static_assert(std::is_same<decltype(U{}*X{}+V{}*Y{}), decltype(UV{}*XY{})>::value,
		"Operator* must be distributive over operator+");
	oss << "U*X+V*Y = (U+V)*(X+Y) = " << pretty(U{}*X{}+V{}*Y{}) << ".\n";

	static_assert(std::is_same<decltype(E{}^E{}), E>::value
			&& std::is_same<decltype(E{}^X{}), E>::value
			&& std::is_same<decltype(U{}^E{}), E>::value,
		"The empty tag must be the multiplicative zero");
	oss << "Outer product (operator^, operator%):" << para
			<< "0 ^ X = " << pretty(E{}^X{}) << ";" << para
			<< "U ^ 0 = " << pretty(U{}^E{}) << ";" << para
			<< "U ^ X = " << pretty(U{}^X{}) << ";" << para
			<< "(U+V) ^ X = " << pretty(UV{} ^ X{}) << ";" << para
			<< "U ^ (X+Y) = " << pretty(U{} ^ XY{}) << ";" << para
			<< "(U+V) ^ (X+Y) = " << pretty(UV{} ^ XY{}) << "." << para
			<< "(U+V+W) ^ (X+Y) = " << pretty(UVW{} ^ XY{}) << "." << para
			<< "(U+V) ^ (X+Y+Z) = " << pretty(UV{} ^ XYZ{}) << "." << para
			<< "(U+V+W) ^ (X+Y+Z) = " << pretty(UVW{} ^ XYZ{}) << ".\n"
		"Transposed outer product:" << para
			<< "0 % X = " << pretty(E{} % X{}) << ";" << para
			<< "U % 0 = " << pretty(U{} % E{}) << ";" << para
			<< "U % X = " << pretty(U{} % X{}) << ";" << para
			<< "(U+V) % X = " << pretty((UV{}) % X{}) << ";" << para
			<< "U % (X+Y) = " << pretty(U{} % XY{}) << ";" << para
			<< "(U+V) % (X+Y) = " << pretty(UV{} % XY{}) << ";" << para
			<< "(U+V+W) % (X+Y+Z) = " << pretty(UVW{} % XYZ{}) << ".\n"
		"Evaluation:" << para
			<< "[0] = " << pretty(eval(E{})) << ";" << para
			<< "[U] = " << pretty(eval(U{})) << ";" << para
			<< "[U+V] = " << pretty(eval(UV{})) << ";" << para
			<< "[(U+V)^(X+Y)] = " << pretty(eval(UV{}^XY{})) << ".\n";
	auto uv = eval(UV{});
	static_assert(std::is_same<decltype(access<0>(uv)), u&>::value
			&& std::is_same<decltype(access<1>(uv)), v&>::value,
		"Access<I> must emulate std::get<I>");
	oss << "Access:" << para
			<< "[U+V]_0 = " << pretty(access<0>(eval(UV{}))) << ";" << para
			<< "[U+V]_1 = " << pretty(access<1>(eval(UV{}))) << ";" << para
			<< "[(U+V)^(X+Y)]_1 = " << pretty(access<1>(eval(UV{}^XY{}))) << ";" << para
			<< "[(U+V)^(X+Y)]_1_1 = " << pretty(access<1,1>(eval(UV{}^XY{}))) << ";" << para
			<< "[U+V]_0 . 1 = " << (access<0>(uv) = 1) << ";" << para
			<< "[U+V]_1 . 2 = " << (access<1>(uv) = 2) << ".\n";

	static_assert(std::is_same<decltype(reverse(UVW{})), decltype(W{}+V{}+U{})>::value,
		"Reverse must distribute over addition");
	static_assert(std::is_same<decltype(reverse(UV{}^XY{})),
				decltype(reverse(UV{})^reverse(XY{}))>::value
			&& std::is_same<decltype(reverse(UV{}*XY{})),
				decltype(reverse(UV{})*reverse(XY{}))>::value,
		"Reverse must distribute over multiplication");
	oss << "Reversal:" << para
			<< "rev(U+V) = " << pretty(reverse(UV{})) << ";" << para
			<< "rev(U+V+W) = " << pretty(reverse(UVW{})) << ";" << para
			<< "rev((U+V)^(X+Y)) = " << pretty(reverse(UV{}^XY{})) << ".\n";
	oss << "Collapse:" << para
			<< "collapse(" << pretty(E{}) << ") = "
				<< pretty(collapse(E{})) << ";" << para
			<< "collapse(" << pretty(F{}) << ") = "
				<< pretty(collapse(F{})) << ";" << para
			<< "collapse(" << pretty(F{}+D{}) << ") = "
				<< pretty(collapse(F{}+D{})) << ";" << para
			<< "collapse(" << pretty((F{}+D{})^(D{}+F{})) << ") = "
				<< pretty(collapse((F{}+D{})^(D{}+F{}))) << ";" << para
			<< "collapse(" << pretty(Tag<decltype((F{}+D{})^(D{}+F{}))>{}) << ") = "
				<< pretty(collapse(Tag<decltype((F{}+D{})^(D{}+F{}))>{})) << ".\n";
	/*oss << "Deval:" << para
			<< "deval(" << pretty(eval(F{})) << ") = "
				<< pretty(deval(eval(F{}))) << ";" << para
			<< "deval(" << pretty(F{}) << ") = "
				<< pretty(deval(F{})) << ";" << para
			<< "deval(" << pretty(eval(U{})) << ") = "
				<< pretty(deval(eval(U{}))) << ".\n";*/
	static_assert(std::is_same<decltype(reverse(UV{})), decltype(rotate(UV{}))>::value,
		"Rotate must move the head type to the end of the tail");
	static_assert(std::is_same<UV, decltype(rotate(rotate(UV{})))>::value,
		"N rotations of N elements must be the identity");
	oss << "Rotation:" << para
			<< "rot(U+V+W) = " << pretty(rotate(UVW{})) << ";" << para
			<< "rot^2(U+V+W) = " << pretty(rotate(rotate(UVW{}))) << ";" << para
			<< "rot^3(U+V+W) = " << pretty(rotate(rotate(rotate(UVW{})))) << ".\n"
		"Sequences:" << para
			<< "seq(1,2) = " << pretty(Detail::SeqU<1,2>{}) << ".";

	auto res = oss.str();

	using S2 = array<string, 2>;
	for(auto const& it : vector<S2>{{"->", "-%"}, {"Detail::", ""}, {"Tag", ""},
			{"std::tuple", "Val"}, {"float", "f"}, {"double", "d"},
			{"std::integral_constant<unsigned int, ", "Const<"},
			{"> >", ">>"}, {"<", "{"}, {">", "}"}, {"-%", "->"},
			{"}, ", "},_"}, {", ", ","}, {"},_","}, "}
			}) {
		std::size_t pos;
		while((pos = res.find(it[0])) != std::string::npos)
			res.replace(pos, it[0].length(), it[1]);
	}
	cout << res << endl;
}
