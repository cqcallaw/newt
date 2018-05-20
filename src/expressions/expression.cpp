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
#include <primitive_type.h>
#include <complex_type.h>

Expression::Expression(const yy::location location) :
		m_location(location) {
}

Expression::~Expression() {
}

TypedResult<string> Expression::ToString(
		const shared_ptr<ExecutionContext> execution_context) const {
	ostringstream buffer;
	const_shared_ptr<Result> evaluation = Evaluate(execution_context,
			execution_context);
	auto errors = evaluation->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto type_specifier_result = GetTypeSpecifier(execution_context,
				AliasResolution::RESOLVE);

		errors = type_specifier_result.GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto type_specifier = type_specifier_result.GetData();
			auto type_result = type_specifier->GetType(
					execution_context->GetTypeTable(),
					AliasResolution::RESOLVE);

			errors = type_result->GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				auto type = type_result->GetData<TypeDefinition>();
				auto as_primitive = std::dynamic_pointer_cast<
						const PrimitiveType>(type);
				if (as_primitive) {
					const BasicType basic_type = as_primitive->GetType();
					switch (basic_type) {
					case BOOLEAN:
						buffer << *(evaluation->GetData<bool>());
						break;
					case BYTE: {
						buffer << std::hex << uppercase
								<< unsigned(
										*(evaluation->GetData<std::uint8_t>()));
						break;
					}
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
				} else {
					auto type_specifier_mapping =
							TypeSpecifier::DefaultTypeSpecifierMap;
					if (!TypeSpecifierList::IsTerminator(
							type_specifier->GetTypeArgumentList())) {
						auto type_specifier_mapping_result =
								ComplexType::GetTypeParameterMap(
										type->GetTypeParameterList(),
										type_specifier->GetTypeArgumentList(),
										execution_context->GetTypeTable());

						errors = type_specifier_mapping_result.GetErrors();
						type_specifier_mapping =
								type_specifier_mapping_result.GetData(); // this should be null if we have errors, but the next error list terminator check will handle this case
					}

					if (ErrorList::IsTerminator(errors)) {
						buffer
								<< type->ValueToString(
										execution_context->GetTypeTable(),
										type_specifier_mapping, Indent(0),
										evaluation->GetRawData());
					}
				}
			}
		}
	} else {
		return TypedResult<string>(nullptr, errors);
	}

	return TypedResult<string>(
			const_shared_ptr<string>(new string(buffer.str())), errors);
}
