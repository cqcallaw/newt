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

 When printing large objects (such as a symbol table or nested objects),
 indentation can be very helpful. The Indent provides a simple way to automate
 indentation

 How to use:

 If you include indent.h, you will have access to the global
 variable: Indent indent

 Whenever you start a new line of output, output indent first.
 When you want to increase and decrease the level of intention
 call indent++ or indent--

 Here is some sample code using class Indent

 // indent the following using the "current" indentation level
 os << indent << "Symbol_table" << endl;
 os << indent << "{" << endl;
 indent++; // increase the "current" indentation level
 os << indent << something_goes_here << endl;
 os << indent << something_goes_here << endl;
 os << indent << something_goes_here << endl;
 indent--; // decrease the "current" indentation level
 os << "}" << endl;

 When the entire program shares a single indent object (e.g. the global
 one declared in indent.cpp), you can print nested structures and the
 indentation will always be correct.
 */

#ifndef INDENT_H
#define INDENT_H

#include <iostream>

using namespace std;

class Indent {
public:
	Indent(const int level);

	const Indent operator+(int i) const {
		return Indent(m_level + i);
	}

	const Indent operator-(int i) const {
		int new_level = m_level - 1;
		if (new_level < 0)
			new_level = 0;
		return Indent(new_level);
	}

	// overload pre-fix ++ and --
	const Indent operator++() const {
		return Indent(m_level + 1);
	}

	const Indent operator--() const {
		int new_level = m_level - 1;
		if (new_level < 0)
			new_level = 0;
		return Indent(new_level);
	}

	// overload post-fix ++ and --
	// this is weird, but putting that int there makes it post-fix
	const Indent operator++(int) const {
		return Indent(m_level + 1);
	}

	const Indent operator--(int) const {
		int new_level = m_level - 1;
		if (new_level < 0)
			new_level = 0;
		return Indent(new_level);
	}

	const Indent operator+=(int i) const {
		int new_level = m_level - i;
		return Indent(new_level);
	}

	const Indent operator-=(int i) const {
		int new_level = m_level - i;
		if (new_level < 0)
			new_level = 0;
		return Indent(new_level);
	}

	const string ToString() const;

private:
	const int m_level;
};

// overload the << operator for Indent
std::ostream &operator<<(std::ostream &os, const Indent &indent);

extern Indent indent;

#endif // #ifndef INDENT_H
