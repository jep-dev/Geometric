#ifndef FORMAT_HPP
#define FORMAT_HPP

#include <string>
#include <vector>
#include <iosfwd>

#include "utilities.hpp"

namespace Streams {
namespace Detail {
	using std::string;
	using std::array;
	using std::vector;
	using std::ostringstream;
	using std::istringstream;
}

std::vector<std::string> split(std::string str) {
	using namespace Detail;
	vector<string> out;
	istringstream iss(str);
	string cur;
	while(std::getline(iss, cur)) {
		out.emplace_back(cur);
	}
	return std::move(out);
}

std::vector<std::string> & align(std::vector<std::string> & lines, int dir, char fill = ' ') {
	using namespace Detail;
	auto lens = minimax(lines);
	for(auto & line : lines) {
		auto len = lens.second - line.length();
		if(len > 0) {
			if(dir == 0) {
				auto llen = len / 2, rlen = len - llen;
				if(rlen > 0) {
					line += string(rlen, fill);
					if(llen > 0) line = string(llen, fill) + line;
				}
			} else {
				string pad(len, fill);
				if(dir < 0) line += pad;
				else line = pad + line;
			}
		}
	}
	return lines;
}

std::vector<std::string> paste(std::string const& lhs, std::string const& rhs,
		char fill = ' ', const char *border = " ") {
	using namespace Detail;
	auto lvec = split(lhs), rvec = split(rhs);
	auto llen = lvec.size(), rlen = rvec.size();
	for(long N = rlen - llen, i = 0; i < N; i++)
		lvec.emplace_back("");
	for(long N = llen - rlen, i = 0; i < N; i++)
		rvec.emplace_back("");
	align(lvec, 1, fill);
	align(rvec, -1, fill);
	for(long unsigned int N = lvec.size(), i = 0; i < N; i++) {
		lvec[i] += border + rvec[i];
	}
	return std::move(lvec);
}
template<class... T> std::string join(T &&... t) {
	using namespace Detail;
	auto vec = paste(std::forward<T>(t)...);
	ostringstream oss;
	for(auto const& line : vec) {
		oss << line << '\n';
	}
	return oss.str();
}

}

#endif
