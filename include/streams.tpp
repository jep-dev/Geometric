#ifndef STREAMS_TPP
#define STREAMS_TPP

#include "streams.hpp"

namespace Streams {

template<class S>
std::ostream& center(std::ostream& out, S const& s, unsigned N, bool leftish) {
	using namespace std;
	ostringstream oss;
	oss << s;
	auto str = oss.str();
	int len = str.length(), dlen = N - len, dlen0 = dlen/2, dlen1 = dlen - dlen0;
	if(leftish) std::swap(dlen0, dlen1);
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

}

#endif
