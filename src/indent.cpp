#include "indent.h"
#include <iostream>

#include "assert.h"
using namespace std;

/* static */int Indent::m_count = 0;

Indent indent; // this is the global indent everyone will use

Indent::Indent() {
	m_level = 0;

	// prevents more than one instance from ever being created
	// not as elegant as implementing a singleton... quick and dirty
	m_count++;
	assert(m_count == 1);
}

ostream & operator<<(ostream &os, const Indent &indent) {
	for (int i = 0; i < indent.m_level; i++)
		os << " ";
	return os;
}
