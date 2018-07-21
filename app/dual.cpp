#ifdef DUAL2
#include "dual2.hpp"
#else
#include "dual.hpp"
#endif

#include "dual.tpp"
#include <iomanip>



int main(int argc, const char *argv[]) {
	DualQuaternion<float> LHS[] = {{1}, {0,1}, {0,0,1}, {0,0,0,1},
			{0,0,0,0,1}, {0,0,0,0,0,1}, {0,0,0,0,0,0,1}, {0,0,0,0,0,0,0,1}};
	for(auto const& lhs : LHS) {
		for(auto const& rhs : LHS) {
			std::cout << std::setw(3) << (lhs * rhs) << " ";
		}
		endl(std::cout);
	}
}
