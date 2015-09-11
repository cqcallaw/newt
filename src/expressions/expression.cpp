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

#include <sstream>
#include "expression.h"
#include "error.h"
#include "variable.h"
#include "utils.h"

Expression::Expression(const YYLTYPE position) :
		m_position(position) {
}

Expression::~Expression() {
}

const Result* Expression::ToString(
		const ExecutionContext* execution_context) const {
	ostringstream buffer;
	const Result* evaluation = Evaluate(execution_context);
	if (evaluation->GetErrors() != LinkedList<const Error*>::Terminator) {
		return evaluation;
	} else {
		const TypeSpecifier* type_specifier = GetType(execution_context);

		if (type_specifier->IsAssignableTo(PrimitiveTypeSpecifier::BOOLEAN)) {
			buffer << *((bool*) evaluation->GetData());
		} else if (type_specifier->IsAssignableTo(
				PrimitiveTypeSpecifier::INT)) {
			buffer << *((int*) evaluation->GetData());
		} else if (type_specifier->IsAssignableTo(
				PrimitiveTypeSpecifier::DOUBLE)) {
			buffer << *((double*) evaluation->GetData());
		} else if (type_specifier->IsAssignableTo(
				PrimitiveTypeSpecifier::STRING)) {
			buffer << *((string*) evaluation->GetData());
		} else {
			assert(false);
		}
	}

	delete (evaluation);

	return new Result(new string(buffer.str()),
			LinkedList<const Error*>::Terminator);
}
