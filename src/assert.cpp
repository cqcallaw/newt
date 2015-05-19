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
#include <stdlib.h>
using namespace std;

extern int line_count;  // from lexer.l

void __pretty_assert(const char *filename, int line, const char *text) {
	cerr << "assertion \"" << text << "\" failed: file \"" << filename
			<< "\", line " << line << ".  Input line " << line_count << "."
			<< endl;

	exit(1);
}
