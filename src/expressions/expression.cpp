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

Expression::Expression(const yy::location position) :
		m_position(position) {
}

Expression::~Expression() {
}

const_shared_ptr<Result> Expression::ToString(
		const_shared_ptr<ExecutionContext> execution_context) const {
	ostringstream buffer;
	const_shared_ptr<Result> evaluation = Evaluate(execution_context);
	if (ErrorListBase::IsTerminator(evaluation->GetErrors())) {
		const_shared_ptr<TypeSpecifier> type_specifier = GetType(
				execution_context);
		auto value = evaluation->GetData();

		const_shared_ptr<PrimitiveTypeSpecifier> as_primitive =
				std::dynamic_pointer_cast<const PrimitiveTypeSpecifier>(
						type_specifier);
		if (as_primitive) {
			const BasicType basic_type = as_primitive->GetBasicType();
			switch (basic_type) {
			case BOOLEAN:
				buffer << *(static_pointer_cast<const bool>(value));
				break;
			case INT:
				buffer << *(static_pointer_cast<const int>(value));
				break;
			case DOUBLE:
				buffer << *(static_pointer_cast<const double>(value));
				break;
			case STRING:
				buffer << *(static_pointer_cast<const string>(value));
				break;
			default:
				assert(false);
			}
		}

		//TODO: array printing

		const_shared_ptr<CompoundTypeSpecifier> as_compound =
				std::dynamic_pointer_cast<const CompoundTypeSpecifier>(
						type_specifier);
		if (as_compound) {
			auto instance = static_pointer_cast<const CompoundTypeInstance>(
					value);

			buffer << "{" << endl;
			buffer
					<< instance->ToString(*execution_context->GetTypeTable(),
							Indent(1));
			buffer << "}" << endl;
		}
	} else {
		return evaluation;
	}

	

	return make_shared<Result>(const_shared_ptr<void>(new string(buffer.str())),
			ErrorListBase::GetTerminator());
}
