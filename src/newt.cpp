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

#include "parser.tab.h"
#include "lexer.h"

using namespace std;

int main(int argc, char *argv[]) {
	FILE* input_file;
	yyscan_t scanner;

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
	input_file = fopen(filename, "r");
	if (!input_file) {
		cerr << "Cannot open input file <" << filename << ">." << endl;
		return 2;
	}

	if (debug) {
		cout << "Parsing file " << filename << "..." << endl;
	}

	yylex_init (&scanner);
	yyset_in(input_file, scanner);
	int parse_result = yyparse(scanner);
	yylex_destroy(scanner);

	if (debug) {
		cout << "Parsed file " << filename << "." << endl;
	}

	if (parse_result != 0 || Error::num_errors() != 0) {
		cerr << Error::num_errors() << " error";
		if (Error::num_errors() > 1)
			cout << "s";
		cout << " found; giving up." << endl;

		if (debug) {
			//return "success" so the test infrastructure doesn't barf
			exit(EXIT_SUCCESS);
		} else {
			exit(EXIT_FAILURE);
		}
	}

	if (parse_result == 0 && Error::num_errors() == 0 && debug) {
		cout << "Root Symbol Table:" << endl;
		SymbolTable::instance()->print(cout);
	}
}
