#include <sstream>
#include <numeric>

#include "reader.hpp"

namespace Streams {
	std::size_t Reader::size(void) const { return lines.size(); }
	Reader::operator std::string(void) const {
		using std::string;
		return std::accumulate(lines.begin(), lines.end(), string(),
			[](string const& cur, string const& s) { return cur + "\n" + s; });
	}
}
