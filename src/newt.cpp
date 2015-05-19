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
