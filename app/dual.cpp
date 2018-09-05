#include <algorithm>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>

#ifdef DUAL2
#include "dual2.hpp"
#else
#include "dual.hpp"
#endif

#include "dual.tpp"

#ifndef DUAL_IMPL
#define DUAL_IMPL "unknown"
#endif

#include "streams.tpp"
#include "utility.hpp"

template<class T, unsigned N, unsigned M>
std::vector<std::string> get_table(std::string const& title,
		const T (&LHS)[N], const T (&RHS)[M], T (*fn)(T const&, T const&)) {
	using namespace std;
	using Streams::center;

	vector<string> out;
	ostringstream os;
	char b_n = '-', b_ne = '.', b_e = '|', b_se = '\'',
			b_s = '-', b_sw = '\'', b_w = '|', b_nw = '.',
			b_c = '+', b_none = ' ';
	string utitle = "(u)", vtitle = "(v)", pad = " ";
	long wu = utitle.length(), wv = vtitle.length(), wtitle = title.length(),
			rwv = (wv-1)/2, lwv = wv-1-rwv, wcell = max(3l, wu),
			wpad = pad.length(), wpcell = wcell + wpad * 2, wrow = wpcell * M,
			dtitle = wtitle + 2;
	string lv = string(lwv, ' '), rv = string(rwv, ' '),
			north = b_none + string(wpcell + lwv, ' ') + b_nw
				+ center(center(title, dtitle, ' '), wrow + rwv, '-') + b_ne,
		middle = b_nw + center(utitle, wpcell+lwv, '-', false) + b_c + string(wrow+rwv, '-') + b_c,
		bottom = b_sw + string(wpcell+lwv, '-') + b_c + string(wrow+rwv, '-') + b_se;
	out.emplace_back(north);
	string header = b_none + string(wpcell, ' ') + vtitle;
	for(auto const& rhs : RHS)
		header += pad + center(rhs, wcell, ' ') + pad;
	header += b_e;
	out.emplace_back(header);
	out.emplace_back(middle);
	for(auto const& lhs : LHS) {
		string str = b_w + center(string(lhs), wpcell+lwv, ' ') + b_w + string(rwv, ' ');
		for(auto const& rhs : RHS) {
			str += pad + center(fn(lhs, rhs), wcell, ' ', false) + pad;
		}
		str += b_e;
		out.emplace_back(str);
	}
	out.emplace_back(bottom);
	return out;
}

int main(int argc, const char *argv[]) {
	using namespace std;
	using Streams::center;

	cout << "Using dual quaternions as implemented in " << DUAL_IMPL << '.' << endl;

	typedef float T;
	typedef DualQuaternion<T> DQ;
	typedef DQ (binary) (DQ const&, DQ const&);

	/*DQ p1 = 1_e + 1_I, p2 = 1_e + 1_J, dp = p2 - p1;
	// Line: p1 + dp * t
	DQ x = 1_e + 1_I + 1_J + 1_k, dxp = x - p1,
		projection = p1 + dp * dot(dxp, dp),
		rejection = dxp - projection;
	cout << "Line l from " << p1 << " to " << p2 << " is given by l(t) = "
			<< p1 << " + (" << dp << ")t" << endl;
	cout << "Point x = " << x << " is " << dxp << " from " << p1 << endl;
	cout << "  Projection = " << projection << "\n"
			"  Rejection = " << rejection << endl;

	return 0;*/

	DQ LHS[] = {1_e, 1_i, 1_j, 1_k, 1_E, 1_I, 1_J, 1_K},
		RHS[] = {1_i, 1_i + 1_J /* (1_i + 1_j)*T(sqrtf(2)/2) */ };

	map<string, binary*> functions = {
		{"W(u, v) = uv", [] (DQ const& l, DQ const& r) -> DQ { return l * r; }},
		{"W(u, v) = u v *u", [] (DQ const& l, DQ const& r) -> DQ { return l ^ r; }},
		{"W(u, v) = v u *v", [] (DQ const& l, DQ const& r) -> DQ { return r ^ l; }}
	};
	/*auto ntables = functions.size();
	vector<ostringstream> oss;
	vector<istringstream> iss;
	for(auto i = 0; i < ntables; i++) {
		oss.emplace_back();
		print_table(oss[i], LHS, LHS, functions[i]);
		iss.emplace_back(oss[i].str());
	}*/

	cout << "Sandwich operators: " << endl;
	vector<vector<std::string>> lines;
	for(auto const& fn : functions)
		lines.emplace_back(get_table(fn.first, LHS, LHS, fn.second));
	for(long k = 0, dk = 2, n = lines.size(); k < n; k += dk) {
		for(long j = 0, m = lines[0].size(); j < m; j++) {
			for(long i = k; i < k+dk && i < n; i++) {
				cout << lines[i][j] << "  ";
			}
			cout << endl;
		}
	}
		/*string line;
		bool cont = true;
		for(unsigned i = 0; i < ntables; i++) {
			if(i) cout << "  ";
			if(!std::getline(iss[i], line)) {
				cont = false;
				break;
			}
			cout << line;
		}
		cout << endl;
		if(!cont) break;
	}*/

/*std::vector<std::vector<std::string>> cols = {
	{"u"}, {"v"}, {"uv"}, {"uvu*"},
	{"v"}, {"u"}, {"vu"}, {"vuv*"}
};
for(auto const& lhs : LHS) {
	for(auto const& rhs : LHS) {
		cols[0].emplace_back(to_string(lhs, 3));
		cols[1].emplace_back(to_string(rhs, 3));
		cols[2].emplace_back(to_string(rhs * lhs, 3));
		cols[3].emplace_back(to_string(lhs ^ rhs, 3));
		cols[4].emplace_back(to_string(rhs, 3));
		cols[5].emplace_back(to_string(lhs, 3));
		cols[6].emplace_back(to_string(lhs * rhs, 3));
		cols[7].emplace_back(to_string(rhs ^ lhs, 3));
	}
}
std::vector<long> maxes;
for(long j = 0, m = cols.size(); j < m; j++)
	maxes.emplace_back(minimax(cols[j]).second);
for(long i = 0, n = cols[0].size(); i < n; i++) {
	for(long j = 0, m = cols.size(); j < m; j++) {
		cout << Streams::center(cols[j][i], maxes[j]);
		if(j == 3) cout << "##";
		else cout << "   ";
	}
	if(!i) {
		cout << " |  ";
		for(long j = 0, m = cols.size(); j < m; j++)
			cout << Streams::center(cols[j][i], maxes[j]) << "   ";
	}
	long k = i-1;
	if(k & 1) endl(cout);
	//if(i && !(i&1)) endl(cout);
	else cout << " |  ";
}*/

	/*cout << setw(3) << "" << " | ";
	for(auto const& rhs : LHS)
		cout << setw(3) << rhs << " ";
	cout << endl;
	for(auto const& lhs : LHS) {
		cout << setw(3) << lhs << " | ";
		for(auto const& rhs : LHS) {
			cout << setw(3) << (lhs ^ rhs) << " ";
		}
		cout << endl;
	}*/
	/*DQ LHS[] = {{1}, {0,1}, {0,0,1}, {0,0,0,1},
			{0,0,0,0,1}, {0,0,0,0,0,1}, {0,0,0,0,0,0,1}, {0,0,0,0,0,0,0,1}};*/
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
