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
	auto len = str.length(), dlen = N - len, dlen0 = dlen/2, dlen1 = dlen - dlen0;
	if(leftish) std::swap(dlen0, dlen1);
	return out << setw(dlen0) << "" << str << setw(dlen1) << "";
}
std::string center(std::string const& s, unsigned N, char fill, bool leftish) {
	auto len = s.length(), len2 = N - len, llen = len2/2, rlen = len2 - llen;
	if(leftish) std::swap(llen, rlen);
	std::string lpad(llen, fill), rpad(rlen, fill);
	return lpad + s + rpad;
}

}

#endif
