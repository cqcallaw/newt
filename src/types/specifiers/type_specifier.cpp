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

#include <type_specifier.h>
#include <type_definition.h>
#include <complex_type.h>
#include <unit_type.h>

const_shared_ptr<type_specifier_map> TypeSpecifier::DefaultTypeSpecifierMap =
		make_shared<type_specifier_map>();

const_shared_ptr<void> TypeSpecifier::DefaultValue(
		const TypeTable& type_table) const {
	auto type_result = GetType(type_table, RESOLVE);

	auto errors = type_result->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto type = type_result->GetData<TypeDefinition>();
		return type->GetDefaultValue(type_table,
				TypeSpecifier::DefaultTypeSpecifierMap);
	}

	return nullptr;
}

TypedResult<TypeTable> TypeSpecifier::GetTypeParameterMap(
		const TypeSpecifierListRef type_parameters,
		const TypeSpecifierListRef type_arguments,
		const shared_ptr<TypeTable> type_table) {
	auto errors = ErrorList::GetTerminator();

	auto parameter_subject = type_parameters;
	auto argument_subject = type_arguments;
	volatile_shared_ptr<TypeTable> result = make_shared<TypeTable>(type_table);

	while (!TypeSpecifierList::IsTerminator(argument_subject)
			&& ErrorList::IsTerminator(errors)) {
		auto type_argument = argument_subject->GetData();
		if (!TypeSpecifierList::IsTerminator(parameter_subject)) {
			auto type_parameter = parameter_subject->GetData();
			auto as_complex = dynamic_pointer_cast<const ComplexTypeSpecifier>(
					type_parameter);
			assert(as_complex);
			auto type_parameter_string = as_complex->ToString();

			auto existing_entry = result->GetType<TypeDefinition>(
					type_parameter_string, SHALLOW, RETURN);
			if (!existing_entry) {
				auto entry = make_shared<AliasDefinition>(type_table,
						type_argument, DIRECT, nullptr);
				result->AddType(type_parameter_string, entry);
			} else {
				// duplicate entry
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::PREVIOUS_DECLARATION,
								type_parameter->GetLocation().begin,
								type_parameter_string), errors);
			}
		} else {
			// too many arguments
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::TOO_MANY_ARGUMENTS,
							type_argument->GetLocation().begin), errors);
		}
		argument_subject = argument_subject->GetNext();
		parameter_subject = parameter_subject->GetNext();
	}

	if (!TypeSpecifierList::IsTerminator(parameter_subject)) {
		// not enough arguments
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC, Error::TOO_FEW_ARGUMENTS,
						argument_subject->GetData()->GetLocation().begin),
				errors);
	}

	return TypedResult<TypeTable>(result, errors);
}
