#include <iostream>

#include "error.h"

using namespace std;

extern int yyparse();

int yyerror(const char *str) {
	Error::error(Error::PARSE_ERROR, str);
	return 1;
}

int main(int argc, char *argv[]) {
	extern FILE *yyin;

	if (argc < 2) {
		cerr << "Input script must be specified." << endl;
		return 1;
	}

	char *filename = argv[1];

	yyin = fopen(filename, "r");
	if (!yyin) {
		cerr << "Cannot open input file <" << filename << ">." << endl;
		return 2;
	}

	yyparse();
}
