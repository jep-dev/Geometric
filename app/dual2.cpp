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

	typedef float T;
	typedef Quaternion<T> Q;

	std::ostringstream oss;

	Wrapper<T> wtDef;
	decltype(auto) tDef = unwrap(wtDef);
	oss << Pretty<decltype(wtDef)>() << " -> "
			<< Pretty<decltype(tDef)>() << " -> " << tDef << endl;
	// Dual2::Wrapper<float, void> -> float& -> 0

	Wrapper<T> wtVal = wrap(T(M_PI));
	decltype(auto) tVal = unwrap(wtVal);
	oss << Pretty<decltype(wtVal)>() << " -> "
			<< Pretty<decltype(tVal)>() << " -> " << tVal << endl;
	// Dual2::Wrapper<float, void> -> float& -> 3.14159

	Wrapper<Q> wsDef;
	decltype(auto) sDef = unwrap(wsDef);
	oss << Pretty<decltype(wsDef)>() << " -> "
			<< Pretty<decltype(sDef)>() << " -> " << sDef << endl;
	// Dual2::Wrapper<Quaternion<float>, void> -> Quaternion<float>& -> 0

	Wrapper<Q> wsVal(T(M_PI));    // OK
	// Wrapper<Q> wsVal{T(M_PI)}; // OK
	decltype(auto) sVal = unwrap(wsVal);
	oss << Pretty<decltype(wsVal)>() << " -> "
			<< Pretty<decltype(sVal)>() << " -> " << sVal << endl;
	// Dual2::Wrapper<Quaternion<float>, void> -> Quaternion<float>& -> 3.141e


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

	//Xi xi(1); Yj yj(2); Zk zk(3);
	//QE qE(S{0, 1, 2, 3});
	//QE qE = S{0, 1, 2, 3};
	//  QE qE = QE(0, 1, 2, 3); // No Unit(int...)
	//  QE qE{0, 1, 2, 3};      // No Unit(<brace-list>)
	//  QE qE = {0, 1, 2, 3};

	/*typedef Unit<S, ID> id_t;
	typedef Unit<S, BETA> beta_t;
	typedef Unit<S, EPS> eps_t;

	const id_t id = {1};
	const beta_t beta = {0, 4, 5, 6};
	const eps_t eps = {0, 2, 3, 4};

	auto id1 = id.get<ID>(), // idB = id.get<BETA>(), idE = id.get<EPS>(),
		betaB = beta.get<BETA>(), // beta1 = beta.get<ID>(), betaE = beta.get<EPS>(),
		epsE = eps.get<EPS>(), // eps1 = eps.get<ID>(), epsB = eps.get<BETA>(),
		fId1 = get<ID>(id),
		fBetaB = get<BETA>(beta),
		fEpsE = get<EPS>(eps);*/
	/*oss << id1 << ", " << betaB << ", " << epsE << endl;
	oss << boolalpha << (id1 == fId1) << ", " << (betaB == fBetaB)
		<< ", " << (epsE == fEpsE) << endl;*/

	auto str = oss.str();
	//for(pair<string, string> strip : {{"Dual2::", ""}, {"Detail::", ""}}) {
	//for(auto && strip : std::vector<std::pair<std::string, std::string>>
	for(pair<string,string> strip :
			{make_pair("Dual2::", ""), {"Detail::", ""}, {"> >", ">>"}}) {
		auto pos = str.find(strip.first);
		while((pos = str.find(strip.first, pos)) != string::npos) {
			str.replace(pos, strip.first.length(), strip.second);
			pos++;
		}
	}
	cout << str << endl;
}
