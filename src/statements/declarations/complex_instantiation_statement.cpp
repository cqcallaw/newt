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

#include <complex_instantiation_statement.h>
#include <map>
#include <execution_context.h>
#include <type_table.h>
#include <result.h>
#include <member_instantiation.h>
#include <expression.h>

#include <record_type.h>
#include <symbol.h>
#include <sum_type.h>
#include <sum.h>
#include <complex_type_specifier.h>
#include <placeholder_type.h>
#include <record.h>

ComplexInstantiationStatement::ComplexInstantiationStatement(
		const yy::location position,
		const_shared_ptr<ComplexTypeSpecifier> type_specifier,
		const yy::location type_specifier_location,
		const_shared_ptr<string> name, const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) :
		DeclarationStatement(position, name, name_position,
				initializer_expression, ModifierList::GetTerminator(),
				GetDefaultLocation()), m_type_specifier(type_specifier), m_type_specifier_location(
				type_specifier_location) {
}

ComplexInstantiationStatement::~ComplexInstantiationStatement() {
}

const PreprocessResult ComplexInstantiationStatement::Preprocess(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure,
		const TypeSpecifierListRef type_parameter_list,
		const_shared_ptr<TypeSpecifier> return_type_specifier) const {
	auto type_result = m_type_specifier->GetType(context->GetTypeTable());

	auto errors = type_result->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto type = type_result->GetData<TypeDefinition>();
		auto as_complex = dynamic_pointer_cast<const ComplexType>(type);
		if (as_complex) {
			auto existing = context->GetSymbol(GetName(), SHALLOW);
			if (existing == Symbol::GetDefaultSymbol()) {
				volatile_shared_ptr<type_parameter_map> type_map = make_shared<
						type_parameter_map>();
				if (!TypeSpecifierList::IsTerminator(type_parameter_list)) {
					auto subject = type_parameter_list;
					while (!TypeSpecifierList::IsTerminator(subject)) {
						auto key = subject->GetData()->ToString();
						auto new_entry = std::pair<const string,
								const_shared_ptr<TypeSpecifier>>(key,
								TypeTable::GetNilTypeSpecifier());
						type_map->insert(type_map->end(), new_entry);
						subject = subject->GetNext();
					}
				}
				auto result = as_complex->PreprocessSymbol(context,
						m_type_specifier, GetInitializerExpression(), type_map);

				errors = result->GetErrors();
				if (ErrorList::IsTerminator(errors)) {
					auto symbol = result->GetData<Symbol>();
					if (symbol) {
						auto name = *GetName();
						const InsertResult insert_result =
								context->InsertSymbol(name, symbol);

						if (insert_result != INSERT_SUCCESS) {
							assert(false);
						}
					} else {
						assert(false);
					}
				}
			} else {
				//symbol already exists
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::PREVIOUS_DECLARATION,
								m_type_specifier_location.begin, *GetName()),
						errors);
			}
		} else {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::NOT_A_COMPOUND_TYPE,
							m_type_specifier_location.begin,
							m_type_specifier->ToString()), errors);
		}
	}

	return PreprocessResult(PreprocessResult::ReturnCoverage::NONE, errors);
}

const_shared_ptr<TypeSpecifier> ComplexInstantiationStatement::GetTypeSpecifier() const {
	return m_type_specifier;
}

const ExecutionResult ComplexInstantiationStatement::Execute(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	auto type_result = m_type_specifier->GetType(context->GetTypeTable(),
			RESOLVE);

	auto errors = type_result->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto type = type_result->GetData<TypeDefinition>();
		auto as_complex = dynamic_pointer_cast<const ComplexType>(type);
		if (as_complex) {
			errors = as_complex->Instantiate(context, m_type_specifier,
					GetName(), GetInitializerExpression());
		} else {
			//type does not exist
			errors = ErrorList::From(
					make_shared<Error>(Error::RUNTIME, Error::UNDECLARED_TYPE,
							m_type_specifier_location.begin,
							m_type_specifier->ToString()), errors);
		}
	}

	return ExecutionResult(errors);
}

const yy::location ComplexInstantiationStatement::GetTypeSpecifierLocation() const {
	return m_type_specifier_location;
}

const DeclarationStatement* ComplexInstantiationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
	return new ComplexInstantiationStatement(GetLocation(), m_type_specifier,
			m_type_specifier_location, GetName(), GetNameLocation(), expression);
}
