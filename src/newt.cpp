/*
 Copyright (C) 2015 The newt Authors.

 This file is part of newt.

 newt is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 newt is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with newt.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <string.h>

#include "error.h"
#include "symbol_table.h"

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

	bool debug = false;
	for (int i = 1; i < argc - 1; i++) {
		int compare = strcmp(argv[i], "--debug");
		if (compare == 0) {
			debug = true;
		}
	}

	char *filename = argv[argc - 1];
	yyin = fopen(filename, "r");
	if (!yyin) {
		cerr << "Cannot open input file <" << filename << ">." << endl;
		return 2;
	}

	if (debug) {
		cout << "Parsing file " << filename << "..." << endl;
	}

	int parse_result = yyparse();

	if (debug) {
		cout << "Parsed file " << filename << "." << endl;
	}

	if (parse_result != 0 || Error::num_errors() != 0) {
		cerr << Error::num_errors() << " error";
		if (Error::num_errors() > 1)
			cout << "s";
		cout << " found; giving up." << endl;

		exit (EXIT_FAILURE);
	}

	if (parse_result == 0 && Error::num_errors() == 0 && debug) {
		cout << "Root Symbol Table:" << endl;
		SymbolTable::instance()->print(cout);
	}
}
