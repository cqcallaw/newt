#include <iostream>
#include <stdlib.h>
using namespace std;

extern int line_count;  // from lexer.l

void __pretty_assert(const char *filename, int line, const char *text) {
	cerr << "assertion \"" << text << "\" failed: file \"" << filename
			<< "\", line " << line << ".  Input line " << line_count << "."
			<< endl;

	exit(1);
}
