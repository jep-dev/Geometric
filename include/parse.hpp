#include "streams.hpp"

namespace Streams {

std::string trim(std::string const& in) {
	int p0 = 0, p1 = in.length()-1;
	bool met = false;
	for(int i = 0, n = in.length(); i < n; i++) {
		if(in[i] != ' ') {
			met = true;
			p0 = i;
			break;
		}
	}
	if(!met) return "";
	for(int i = in.length()-1; i >= p0; i--) {
		if(in[i] != ' ') {
			p1 = i;
			break;
		}
	}
	return in.substr(p0, p1-p0+1);
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
		return out.second = trim(stripped), out;
	decltype(pos) p0 = 0, p1 = pos-1, p2 = pos+1, p3 = n-1;
	if(p1 >= p0)
		out.first = trim(stripped.substr(p0, p1-p0+1));
	if(p3 >= p2)
		out.second = trim(stripped.substr(p2, p3-p2+1));
	return out;
}
}
