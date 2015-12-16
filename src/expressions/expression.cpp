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
#include <execution_context.h>

Expression::Expression(const YYLTYPE position) :
		m_position(position) {
}

Expression::~Expression() {
}

const Result* Expression::ToString(
		const ExecutionContext* execution_context) const {
	ostringstream buffer;
	const Result* evaluation = Evaluate(execution_context);
	if (evaluation->GetErrors()->IsTerminator()) {
		const TypeSpecifier* type_specifier = GetType(execution_context);
		const void* value = evaluation->GetData();

		const PrimitiveTypeSpecifier* as_primitive =
				dynamic_cast<const PrimitiveTypeSpecifier*>(type_specifier);
		if (as_primitive) {
			const BasicType basic_type = as_primitive->GetBasicType();
			switch (basic_type) {
			case BOOLEAN:
				buffer << *((bool*) value);
				break;
			case INT:
				buffer << *((int*) value);
				break;
			case DOUBLE:
				buffer << *((double*) value);
				break;
			case STRING:
				buffer << *((string*) value);
				break;
			default:
				assert(false);
			}
		}

		//TODO: array printing

		const CompoundTypeSpecifier* as_compound =
				dynamic_cast<const CompoundTypeSpecifier*>(type_specifier);
		if (as_compound) {
			const CompoundTypeInstance* instance =
					(const CompoundTypeInstance*) value;

			buffer << "{" << endl;
			buffer
					<< instance->ToString(execution_context->GetTypeTable(),
							Indent(1));
			buffer << "}" << endl;
		}
	} else {
		return evaluation;
	}

	delete (evaluation);

	return new Result(new string(buffer.str()),
			LinkedList<const Error*>::GetTerminator());
}
