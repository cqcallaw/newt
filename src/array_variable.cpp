/*
 * array_variable.cpp
 *
 *  Created on: May 18, 2015
 *      Author: caleb
 */

#include <array_variable.h>
#include <expression.h>
#include <sstream>

#include "assert.h"

ArrayVariable::ArrayVariable(const string* name,
		const Expression* index_expression) :
		Variable(name), m_index_expression(index_expression) {
	assert(index_expression != NULL && index_expression != nullptr);
	assert(index_expression->GetType() == INT);
}

const Type ArrayVariable::GetType() const {
	switch (Variable::GetType()) {
	case INT_ARRAY:
		return INT;
	case DOUBLE_ARRAY:
		return DOUBLE;
	case STRING_ARRAY:
		return STRING;
	default:
		assert(false);
		return NONE;
	}
}

const string* ArrayVariable::ToString() const {
	ostringstream buffer;
	buffer << "<" << *GetName() << "["
			<< *((int*) m_index_expression->Evaluate()) << "]>";
	return new string(buffer.str());
}

std::ostream &operator<<(std::ostream &os,
		const ArrayVariable &array_variable) {
	os << *(array_variable.ToString());
	return os;
}

ArrayVariable::~ArrayVariable() {
}
