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
#include <record.h>
#include <sum_recursive_type.h>

ComplexInstantiationStatement::ComplexInstantiationStatement(
		const yy::location position,
		const_shared_ptr<ComplexTypeSpecifier> type_specifier,
		const yy::location type_position, const_shared_ptr<string> name,
		const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) :
		DeclarationStatement(position, name, name_position,
				initializer_expression, ModifierList::GetTerminator(),
				GetDefaultLocation()), m_type_specifier(type_specifier), m_type_position(
				type_position) {
}

ComplexInstantiationStatement::~ComplexInstantiationStatement() {
}

const ErrorListRef ComplexInstantiationStatement::preprocess(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();
	//TODO: validate that all members are initialized for readonly structs (?)

	auto type = m_type_specifier->GetType(execution_context->GetTypeTable());
	if (type) {
		auto as_complex = dynamic_pointer_cast<const ComplexType>(type);

		if (as_complex) {
			auto existing = execution_context->GetSymbol(GetName(), SHALLOW);
			if (existing == Symbol::GetDefaultSymbol()) {
				auto result = as_complex->PreprocessSymbol(execution_context,
						m_type_specifier, GetInitializerExpression());

				errors = result->GetErrors();
				if (ErrorList::IsTerminator(errors)) {
					auto symbol = result->GetData<Symbol>();
					if (symbol) {
						auto name = *GetName();
						const InsertResult insert_result =
								execution_context->InsertSymbol(name, symbol);

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
								m_type_position.begin.line,
								m_type_position.begin.column, *GetName()),
						errors);
			}
		} else {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::NOT_A_COMPOUND_TYPE,
							m_type_position.begin.line,
							m_type_position.begin.column,
							m_type_specifier->ToString()), errors);
		}
	} else {
		//type does not exist
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC, Error::UNDECLARED_TYPE,
						m_type_position.begin.line,
						m_type_position.begin.column,
						m_type_specifier->ToString()), errors);
	}

	return errors;
}

const_shared_ptr<TypeSpecifier> ComplexInstantiationStatement::GetTypeSpecifier() const {
	return m_type_specifier;
}

const ErrorListRef ComplexInstantiationStatement::execute(
		shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	const_shared_ptr<TypeDefinition> type = m_type_specifier->GetType(
			execution_context->GetTypeTable(), RESOLVE);

	auto as_complex = dynamic_pointer_cast<const ComplexType>(type);
	if (as_complex) {
		errors = as_complex->Instantiate(execution_context, m_type_specifier,
				GetName(), GetInitializerExpression());
	} else {
		//type does not exist
		errors = ErrorList::From(
				make_shared<Error>(Error::RUNTIME, Error::UNDECLARED_TYPE,
						m_type_position.begin.line,
						m_type_position.begin.column,
						m_type_specifier->ToString()), errors);
	}

	return errors;
}

const DeclarationStatement* ComplexInstantiationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
	return new ComplexInstantiationStatement(GetPosition(), m_type_specifier,
			m_type_position, GetName(), GetNamePosition(), expression);
}
