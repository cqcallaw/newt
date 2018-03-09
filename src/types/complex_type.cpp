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

#include <complex_type.h>
#include <specifiers/type_specifier.h>
#include <type_table.h>
#include <sum_type_specifier.h>
#include <record_type_specifier.h>
#include <expression.h>
#include <unit_type.h>

const_shared_ptr<type_parameter_map> ComplexType::DefaultTypeParameterMap =
		make_shared<type_parameter_map>();

ComplexType::~ComplexType() {
}

const_shared_ptr<Result> ComplexType::PreprocessSymbol(
		const std::shared_ptr<ExecutionContext> execution_context,
		const_shared_ptr<ComplexTypeSpecifier> type_specifier,
		const_shared_ptr<Expression> initializer) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	plain_shared_ptr<void> value;
	plain_shared_ptr<Symbol> symbol = Symbol::GetDefaultSymbol();

	auto type_table = execution_context->GetTypeTable();
	if (initializer) {
		errors = initializer->Validate(execution_context);
		if (ErrorList::IsTerminator(errors)) {
			return PreprocessSymbolCore(execution_context, type_specifier,
					initializer);
		}
	} else {
		auto type_result = type_specifier->GetType(type_table);
		errors = type_result->GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto type = type_result->GetData<TypeDefinition>();
			auto type_mapping_result = ComplexType::GetTypeParameterMap(
					type->GetTypeParameterList(),
					type_specifier->GetTypeArgumentList(), type_table);
			errors = type_mapping_result.GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				value = type->GetDefaultValue(type_table,
						type_mapping_result.GetData());
			}
		}
	}

	if (ErrorList::IsTerminator(errors)) {
		//we've been able to get a good initial value (that is, no errors have occurred)
		auto type_mapping_result = ComplexType::GetTypeParameterMap(
				this->GetTypeParameterList(),
				type_specifier->GetTypeArgumentList(), type_table);
		errors = type_mapping_result.GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto type_map = type_mapping_result.GetData();
			symbol = GetSymbol(type_table, type_specifier, value, type_map);
		}
	}

	return make_shared<Result>(symbol, errors);
}

const ErrorListRef ComplexType::Instantiate(
		const std::shared_ptr<ExecutionContext> execution_context,
		const_shared_ptr<ComplexTypeSpecifier> type_specifier,
		const_shared_ptr<std::string> instance_name,
		const_shared_ptr<Expression> initializer) const {
	ErrorListRef errors = ErrorList::GetTerminator();
	SetResult set_result = SetResult::NO_SET_RESULT;
	yy::location location = GetDefaultLocation();
	plain_shared_ptr<TypeSpecifier> initializer_type_specifier = nullptr;
	if (initializer && !initializer->IsConstant()) {
		location = initializer->GetLocation();

		auto initializer_type_specifier_result = initializer->GetTypeSpecifier(
				execution_context);
		errors = initializer_type_specifier_result.GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto initializer_type_specifier =
					initializer_type_specifier_result.GetData();
			auto result = initializer->Evaluate(execution_context,
					execution_context);
			errors = result->GetErrors();

			if (ErrorList::IsTerminator(errors)) {
				set_result = InstantiateCore(execution_context, type_specifier,
						initializer_type_specifier, *instance_name,
						result->GetData<void>());
			}
		}
	}

	if (set_result) {
		errors =
				ToErrorListRef(set_result, location, instance_name,
						execution_context->GetSymbol(*instance_name)->GetTypeSpecifier(),
						initializer_type_specifier); //initializer_type will be null if initializer is not set
	}

	return errors;
}

TypedResult<type_parameter_map> ComplexType::GetTypeParameterMap(
		const TypeSpecifierListRef type_parameters,
		const TypeSpecifierListRef type_arguments,
		const TypeTable& type_table) {
	auto errors = ErrorList::GetTerminator();

	auto parameter_subject = type_parameters;
	auto argument_subject = type_arguments;
	volatile_shared_ptr<type_parameter_map> result = make_shared<
			type_parameter_map>();

	while (!TypeSpecifierList::IsTerminator(argument_subject)
			&& ErrorList::IsTerminator(errors)) {
		auto type_argument = argument_subject->GetData();
		if (!TypeSpecifierList::IsTerminator(parameter_subject)) {
			auto type_parameter = parameter_subject->GetData();
			auto type_parameter_string = type_parameter->ToString();
			auto existing_entry = result->find(type_parameter_string);
			if (existing_entry == result->end()) {
				auto entry = std::pair<const string,
						const_shared_ptr<TypeSpecifier>>(type_parameter_string,
						type_argument);
				result->insert(result->end(), entry);
			} else {
				// entry already exists
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

	return TypedResult<type_parameter_map>(result, errors);
}

std::string ComplexType::TypeSpecifierListToString(
		const TypeSpecifierListRef type_specifier_list) {
	ostringstream os;
	auto type_parameter_subject = type_specifier_list;
	while (!TypeSpecifierList::IsTerminator(type_parameter_subject)
			&& !TypeSpecifierList::IsTerminator(
					type_parameter_subject->GetNext())) {
		os << type_parameter_subject->GetData()->ToString() << ", ";
		type_parameter_subject = type_parameter_subject->GetNext();
	}

	os << type_parameter_subject->GetData()->ToString();

	return os.str();
}
