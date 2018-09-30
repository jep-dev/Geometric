#ifndef STREAMS_TPP
#define STREAMS_TPP

#include "streams.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace Streams {

template<class S>
ostream& center(ostream& out, S const& s, unsigned N, bool leftish) {
	using namespace std;
	ostringstream oss;
	oss << s;
	auto str = oss.str();
	int len = str.length(), dlen = N - len, dlen0 = dlen/2, dlen1 = dlen - dlen0;
	if(leftish) swap(dlen0, dlen1);
	if(dlen0 > 0) out << setw(dlen0) << "";
	out << str;
	if(dlen1 > 0) out << setw(dlen1) << "";
	return out;
}
std::string center(std::string const& s, unsigned N, char fill, bool leftish) {
	int len = s.length(), len2 = N - len, llen = len2/2, rlen = len2 - llen;
	if(leftish) std::swap(llen, rlen);
	return std::string(std::max(llen, 0), fill) + s + std::string(std::max(rlen, 0), fill);
}

template<class T, class V>
std::pair<long, long> MiniMax<T, V, Detail::Void_t<decltype(&V::tellp)>>::operator()(T& t) const {
	std::pair<long, long> out(std::numeric_limits<long>::max(), 0);
	for(auto& i : t) {
		auto pos = i.tellp();
		if(pos < 0) continue;
		i.seekp(0, std::ios_base::end);
		auto len = i.tellp();
		i.seekp(pos);
		if(len < 0) continue;
		if(len < out.first) out.first = len;
		if(len > out.second) out.second = len;
	}
	return out;
};

template<class S, class SV>
auto level(S& s, char fill)
		-> std::enable_if_t<HasInsert<SV>::value && HasTellpSeekp<SV>::value, S>& {
	auto mm = minimax(s);
	for(auto& it : s) {
		auto cur = it.tellp();
		it.seekp(0, std::ios_base::end);
		auto len = mm.second - it.tellp();
		it.seekp(cur);
		if(len > 0) it += std::string(len, fill);
	}
	return s;
}

struct OStringStream : std::ostringstream {
	std::size_t size(void) const {
		auto s = str();
		return s.length();
	}
	std::size_t size(void) {
		auto pos = tellp();
		seekp(0, std::ios_base::end);
		auto out = tellp();
		seekp(pos);
		return std::max(0, int(out));
	}
	virtual ~OStringStream(void) {}
};

}

#endif
