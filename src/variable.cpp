/*
 * variable.cpp0
 *
 *  Created on: Feb 17, 2015
 *      Author: caleb
 */

#include <sstream>
#include <defaults.h>

#include "variable.h"
#include "assert.h"
#include "expression.h"

Variable::Variable(const string* name) :
		m_name(name) {
	assert(name != NULL && name != nullptr);
}

const Type Variable::GetType() const {
	return Symbol_table::instance()->GetSymbol(m_name)->GetType();
}

const string* Variable::ToString() const {
	ostringstream buffer;
	buffer << "<" << *m_name << ">";
	return new string(buffer.str());
}

std::ostream &operator<<(std::ostream &os, const Variable &variable) {
	os << *(variable.ToString());
	return os;
}

Variable::~Variable() {
}
