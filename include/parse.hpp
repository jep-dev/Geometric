#include "streams.hpp"

namespace Streams {

template<class S, class T>
auto parse_value(S const& s, T const& def)
		-> std::enable_if_t<std::is_arithmetic<T>::value, T> {
	istringstream iss(s);
	T local = def;
	iss >> local;
	return iss.fail() ? def : local;
}
template<class S, class T>
auto parse_positive(S const& s, T const& def)
		-> std::enable_if_t<std::is_arithmetic<T>::value, T> {
	auto value = parse_value(s, def);
	return (value > 0) ? value : def;
}

std::vector<std::string> split_outer(std::string const& in) {
	std::vector<std::string> out;
	unsigned level = 0, accum = 0;
	for(int i = 0, n = in.length(); i < n; i++) {
		switch(in[i]) {
			case '(':
				if(!level && accum) {
					out.emplace_back(trim(in.substr(i-accum, accum)));
					accum = 0;
				}
				if(level) {
					accum++;
				}
				level++;
				break;
			case ')':
				if(level) level--;
				if(accum && !level) {
					out.emplace_back(trim(in.substr(i-accum, accum)));
					accum = 0;
				}
				if(level) accum++;
				break;
			default:
				accum++;
				break;
		}
		if(accum && (i == (n-1))) {
			out.emplace_back(trim(in.substr(i-accum+1, accum)));
		}
	}
	return out;
}
std::pair<std::string, std::string> split_assign(std::string const& in) {
	std::pair<std::string, std::string> out;
	std::string stripped = trim(in);
	auto pos = stripped.find("="), n = stripped.length();
	if(pos == std::string::npos)
		return out.second = stripped, out;
	int p0 = 0, p1 = pos-1, p2 = pos+1, p3 = n-1;
	if(p1 >= p0)
		out.first = trim(stripped.substr(p0, p1-p0+1));
	if(p3 >= p2)
		out.second = trim(stripped.substr(p2, p3-p2+1));
	return out;
}
}
