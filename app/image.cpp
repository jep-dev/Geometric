#include <iostream>
#include <fstream>

#include "image.hpp"

int main(int argc, const char *argv[]) {
	if(argc > 2) {
		std::fstream fs;
		fs.open(argv[1], std::fstream::out);
		if(fs.good()) {
			fs.close();
			fs.open(argv[2], std::fstream::in);
			if(fs.good()) {
				fs.close();
				Image image(argv[2]);
				image.write(argv[1]);
			}
		}
	}
}
