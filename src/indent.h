/***
 Overview:

 When printing large objects (such as a Symbol_table) and nested objects
 indentation can be very helpful.

 class Indent provides an easy way to perform indentation

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

 ***/

#ifndef INDENT_H
#define INDENT_H

#include <iostream>

class Indent {
	friend std::ostream &operator<<(std::ostream &os, const Indent &indent);

public:
	Indent();

	// overload pre-fix ++ and --
	Indent &operator++() {
		m_level += 2;
		return *this;
	}
	Indent &operator--() {
		m_level -= 2;
		if (m_level < 0)
			m_level = 0;
		return *this;
	}

	// overload post-fix ++ and --
	// this is weird, but putting that int there makes it post-fix
	Indent &operator++(int) {
		m_level += 2;
		return *this;
	}
	Indent &operator--(int) {
		m_level -= 2;
		if (m_level < 0)
			m_level = 0;
		return *this;
	}

	Indent &operator+=(int i) {
		m_level += i;
		return *this;
	}
	Indent &operator-=(int i) {
		m_level -= i;
		if (m_level < 0)
			m_level = 0;
		return *this;
	}

private:
	// disable default copy constructor and default assignment
	// done as a precaution, they should never be called
	Indent(const Indent&);
	const Indent &operator=(const Indent&);

	static int m_count;

	int m_level;
};

// overload the << operator for Indent
std::ostream &operator<<(std::ostream &os, const Indent &indent);

// since this file is included by other .cpp file, the following will
// all the .cpp file to access the global indent object that is declared
// in indent.cpp
extern Indent indent;

#endif // #ifndef INDENT_H
