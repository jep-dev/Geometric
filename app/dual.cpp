#include "dual.tpp"
#include <iomanip>

int main(int argc, const char *argv[]) {
	typedef DualQuaternion<float> Q;
	//DualQuaternion<float> lhs = {1, 2, 3, 4, 5, 6, 7, 8}, rhs = {8, 7, 6, 5, 4, 3, 2, 1};;
	//DualQuaternion<float> lhs = {1}, rhs = {0,1};
	Q LHS[] = {{1}, {0,1}, {0,0,1}, {0,0,0,1},
			{0,0,0,0,1}, {0,0,0,0,0,1}, {0,0,0,0,0,0,1}, {0,0,0,0,0,0,0,1}};
	for(Q const& lhs : LHS) {
		for(Q const& rhs : LHS) {
			std::cout << std::setw(3) << (lhs * rhs) << " ";
		}
		endl(std::cout);
	}

}
