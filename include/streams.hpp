#ifndef STREAMS_HPP
#define STREAMS_HPP

#include <iosfwd>
#include <string>
#include <vector>

#include "utility.hpp"

namespace Streams {
using std::string;
using std::vector;
using std::ostream;
using std::istream;
using std::ostringstream;
using std::istringstream;

template<class S>
std::ostream& center(std::ostream& out, S const& s, unsigned N, bool leftish = true);
std::string center(std::string const& out, unsigned N, char fill = ' ', bool leftish = true);

}

#endif
