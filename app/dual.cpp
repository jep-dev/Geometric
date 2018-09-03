#include <algorithm>
#include <iostream>
#include <iomanip>
#include <vector>

#ifdef DUAL2
#include "dual2.hpp"
#else
#include "dual.hpp"
#endif

#include "dual.tpp"

#ifndef DUAL_IMPL
#define DUAL_IMPL "unknown"
#endif

int main(int argc, const char *argv[]) {
	using namespace std;

	typedef float T;
	typedef DualQuaternion<T> DQ;

	cout << "Using " << DUAL_IMPL << " to implement dual quaternions!\n" << endl;

	DQ LHS[] = {{1}, {0,1}, {0,0,1}, {0,0,0,1},
			{0,0,0,0,1}, {0,0,0,0,0,1}, {0,0,0,0,0,0,1}, {0,0,0,0,0,0,0,1}};
	for(auto const& lhs : LHS) {
		for(auto const& rhs : LHS) {
			cout << setw(3) << (lhs * rhs) << " ";
		}
		endl(cout);
	}

	cout << "\nSclerp:\n";
	{
		auto u = LHS[1], v = LHS[2] + 10 * LHS[6];
		//cout << "  For u = " << u << ", v = " << v << "...\n";

		// TODO proper ranging, this is sloppy
		for(T i = 0, di = .1, dj = di/10;
				i < 1 + di - std::numeric_limits<T>::epsilon(); i += di) {
			auto s = sclerp(u, v, i);
			// TODO this too
			for(auto j = 0; j < 8; j++)
				if(s[j]*s[j] < dj*dj) s[j] = 0;
			cout << "  sclerp(" << u << ", " << v << ", " << i << ") = "
					<< s << ";\n";
		}
	}

	cout << "\nBi-sclerp:\n";
	{
		unsigned M = 8, N = 4;
		auto get_uv = [=] (unsigned i, unsigned j) -> std::pair<T, T> {
			float u1 = i/float(M-1),
					v1 = j/float(N-1),
				u2 = 1 - (M-i-1)/float(M-1),
				v2 = 1 - (N-j-1)/float(N-1),
				u = (u1 + u2)/2, v = (v1 + v2)/2;
			return {u, v};
		};
		//DQ l1 = {1}, r1 = {0,1}, l2 = {0,0,1}, r2 = {0,0,0,1};
		DQ l1 = {1},
				r1 = {0,1,0,0,0,0,0,0},
				l2 = {0,0,1,0,0,0,0,0},
				r2 = {0,0,0,1,0,0,0,0};
		vector<vector<string>> cols;
		vector<unsigned> widths;
		for(auto j = 0; j < N; j++) {
			widths.emplace_back();
		}
		ostringstream cell;
		for(auto i = 0; i < M; i++) {
			cols.emplace_back();
			auto & bk = cols.back();
			for(auto j = 0; j < N; j++) {
				bk.emplace_back();
				auto uv = get_uv(i, j);
				auto const& u = uv.first, v = uv.second;
				/*cell << sclerp(l1, r1, l2, r2, u, v);
				auto str = cell.str();
				cell.str("");*/
				std::string str = sclerp(l1, r1, l2, r2, u, v);
				widths[j] = std::max(std::size_t(widths[j]), str.length());
				bk[j] = std::move(str);
			}
		}
		for(auto i = 0; i < M; i++) {
			for(auto j = 0; j < N; j++) {
				auto & col = cols[i][j];
				cout << col << std::string(widths[j] - col.length() + 2, ' ');
			}
			cout << endl;
		}
	}
}
