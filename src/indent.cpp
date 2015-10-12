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

#include "indent.h"
#include <iostream>
#include <sstream>

#include "assert.h"
using namespace std;

Indent::Indent(const int level) :
		m_level(level) {
}

const string Indent::ToString() const {
	ostringstream os;
	for (int i = 0; i < m_level; i++)
		os << "\t";
	return os.str();
}

ostream & operator<<(ostream &os, const Indent &indent) {
	os << indent.ToString();
	return os;
}

