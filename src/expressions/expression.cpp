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
#include <record.h>
#include <primitive_type.h>
#include <record_type.h>

Expression::Expression(const yy::location position) :
		m_position(position) {
}

Expression::~Expression() {
}

const_shared_ptr<Result> Expression::ToString(
		const shared_ptr<ExecutionContext> execution_context) const {
	ostringstream buffer;
	const_shared_ptr<Result> evaluation = Evaluate(execution_context);
	auto errors = evaluation->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto type_specifier = GetTypeSpecifier(execution_context,
				AliasResolution::RESOLVE);
		auto type_result = type_specifier->GetType(
				execution_context->GetTypeTable(), AliasResolution::RESOLVE);

//		//TODO: replace this type switching logic with calls to TypeDefinition::ValueToString()
//		auto type_table = execution_context->GetTypeTable();
//		auto type = type_specifier->GetType(type_table);
//		if (type) {
//			buffer
//					<< type->ValueToString(execution_context->GetTypeTable(),
//							Indent(0), evaluation->GetRawData());
//		} else {
//			assert(false);
//		}

		errors = type_result->GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto type = type_result->GetData<TypeDefinition>();
			auto as_primitive = std::dynamic_pointer_cast<const PrimitiveType>(
					type);
			if (as_primitive) {
				const BasicType basic_type = as_primitive->GetType();
				switch (basic_type) {
				case BOOLEAN:
					buffer << *(evaluation->GetData<bool>());
					break;
				case INT:
					buffer << *(evaluation->GetData<int>());
					break;
				case DOUBLE:
					buffer << *(evaluation->GetData<double>());
					break;
				case STRING:
					buffer << *(evaluation->GetData<string>());
					break;
				default:
					assert(false);
				}
			}

			//TODO: array printing

			auto as_record = std::dynamic_pointer_cast<const RecordType>(type);
			if (as_record) {
				auto instance = evaluation->GetData<Record>();

				buffer << "{" << endl;
				buffer
						<< instance->ToString(
								*execution_context->GetTypeTable(), Indent(1));
				buffer << "}" << endl;
			}
		}
	} else {
		return evaluation;
	}

	return make_shared<Result>(const_shared_ptr<void>(new string(buffer.str())),
			errors);
}
