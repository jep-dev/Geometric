#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "reader.hpp"
#include "utilities.hpp"

namespace Main {
	using std::string;
	using std::cout;
	using std::endl;
	using std::setw;
	using std::left;
	using std::right;
	using std::vector;

	using Streams::Reader;
}

template<class S>
static constexpr const char *pretty(void) {
	return __PRETTY_FUNCTION__;
}
template<class S>
static constexpr const char *pretty(S && s) {
	return __PRETTY_FUNCTION__;
}

int main(int argc, const char *argv[]) {
	using namespace Main;
	string self = argv[0], share, vert, frag;
	string delim = "/", sep = " | ";
	auto pos = self.find_last_of(delim);
	if(pos == string::npos)
		delim = "\\", pos = self.find_last_of(delim);
	if(pos == string::npos)
		return cout << "Could not detect path" << endl, 1;

	if(argc < 3) share = self.substr(0, pos+1) + ".." + delim + "share" + delim;
	vert = (argc < 2) ? share + "default.vert" : argv[1];
	frag = (argc < 3) ? share + "default.frag" : argv[2];
	for(auto const& p : {vert, frag}) {
		cout << "Reading " << p << "; ";

		//vector<string> dest;
		//Streams::readLines(p.c_str(), dest);

		Reader reader(p.c_str());
		auto &dest = reader.get();
		//reader.source(p.c_str(), true);
		if(!reader) {
			cout << "failed!" << endl;
		} else {
			cout << "done (" << dest.size() << " lines)";
			auto flags = cout.flags();
			auto i = 0;
			auto mm = minimax(dest);
			long lw = numDigits(dest.size()), rw = numDigits(mm.second), bw = sep.length(),
				cw = 80 - lw - rw - 2*bw;
			for(auto const& line : dest) {
				cout << '\n' << setw(lw) << i << sep
					<< left << setw(cw) << line << sep
					<< right << setw(rw) << line.length();
				i++;
			}
			cout.setf(flags);
			cout << endl;
		}
	}
}
