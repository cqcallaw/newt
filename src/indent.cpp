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

#include "assert.h"
using namespace std;

/* static */int Indent::m_count = 0;

Indent indent;

Indent::Indent() {
	m_level = 0;

	// prevents more than one instance from ever being created
	// not as elegant as implementing a singleton; quick and dirty
	m_count++;
	assert(m_count == 1);
}

ostream & operator<<(ostream &os, const Indent &indent) {
	for (int i = 0; i < indent.m_level; i++)
		os << " ";
	return os;
}
