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

#include <array_variable.h>
#include <expression.h>
#include <sstream>

#include "yyltype.h"
#include "assert.h"

const Type map_to_primitive(Type type) {
	switch (type) {
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

ArrayVariable::ArrayVariable(const string* name,
YYLTYPE location, const Expression* index_expression,
YYLTYPE expression_location) :
		Variable(name, location), m_index_expression(index_expression), m_expression_location(
				expression_location) {
	/*assert(index_expression != NULL && index_expression != nullptr);assert(
	 index_expression->GetType() == INT);*/
}

const Type ArrayVariable::GetType(const ExecutionContext* context) const {
	Type type = Variable::GetType(context);
	return map_to_primitive(type);
}

const string* ArrayVariable::ToString(const ExecutionContext* context) const {
	ostringstream buffer;
	const Result* evaluation = m_index_expression->Evaluate(context);
	buffer << "<" << *GetName() << "[" << *((int*) evaluation->GetData())
			<< "]>";
	delete (evaluation);
	return new string(buffer.str());
}

ArrayVariable::~ArrayVariable() {
}
