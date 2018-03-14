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

#include <array.h>
#include <error.h>
#include <symbol_table.h>
#include <execution_context.h>
#include <expression.h>
#include <assignment_statement.h>
#include <variable.h>
#include <sstream>
#include <constant_expression.h>
#include <vector>
#include <record_type.h>
#include <array_type_specifier.h>
#include <array_declaration_statement.h>
#include <specifiers/type_specifier.h>
#include <unit_type.h>

ArrayDeclarationStatement::ArrayDeclarationStatement(
		const yy::location position,
		const_shared_ptr<ArrayTypeSpecifier> type_specifier,
		const yy::location type_specifier_location,
		const_shared_ptr<string> name, const yy::location name_location,
		const_shared_ptr<Expression> initializer_expression) :
		DeclarationStatement(position, name, name_location,
				initializer_expression, ModifierList::GetTerminator(),
				GetDefaultLocation()), m_type_specifier(type_specifier), m_type_specifier_location(
				type_specifier_location) {
}

const PreprocessResult ArrayDeclarationStatement::Preprocess(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure,
		const TypeSpecifierListRef type_parameter_list,
		const_shared_ptr<TypeSpecifier> return_type_specifier) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	const_shared_ptr<TypeTable> type_table = context->GetTypeTable();

	const_shared_ptr<ComplexTypeSpecifier> element_type_as_record =
			dynamic_pointer_cast<const ComplexTypeSpecifier>(
					m_type_specifier->GetElementTypeSpecifier());
	if (element_type_as_record) {
		//check that element type exists
		const string type_name = *element_type_as_record->GetTypeName();
		const_shared_ptr<ComplexType> type = type_table->GetType<ComplexType>(
				type_name, DEEP, RETURN);

		if (!type) {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC, Error::UNDECLARED_TYPE,
							m_type_specifier_location.begin, type_name),
					errors);
		}
	}

	if (ErrorList::IsTerminator(errors)) {
		const Array* array = nullptr;
		auto name = GetName();
		auto initializer_expression = GetInitializerExpression();
		if (initializer_expression) {
			auto initializer_expression_type_result =
					initializer_expression->GetTypeSpecifier(context);

			errors = initializer_expression_type_result.GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				auto initializer_expression_type =
						initializer_expression_type_result.GetData();

				const_shared_ptr<ArrayTypeSpecifier> as_array =
						std::dynamic_pointer_cast<const ArrayTypeSpecifier>(
								initializer_expression_type);
				if (!as_array
						|| !initializer_expression_type->AnalyzeAssignmentTo(
								m_type_specifier, *type_table)) {
					errors = ErrorList::From(
							make_shared<Error>(Error::SEMANTIC,
									Error::ASSIGNMENT_TYPE_ERROR,
									initializer_expression->GetLocation().begin,
									m_type_specifier->ToString(),
									initializer_expression_type->ToString()),
							errors);
				}
			}
		}

		if (ErrorList::IsTerminator(errors)) {
			array = new Array(m_type_specifier->GetElementTypeSpecifier(),
					*type_table);
		}

		if (array) {
			auto wrapper = const_shared_ptr<Array>(array);
			auto symbol = const_shared_ptr<Symbol>(new Symbol(wrapper));
			InsertResult insert_result = context->InsertSymbol(*name, symbol);
			if (insert_result == SYMBOL_EXISTS) {
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::PREVIOUS_DECLARATION,
								GetNameLocation().begin, *GetName()), errors);
			}
		}
	}

	return PreprocessResult(PreprocessResult::ReturnCoverage::NONE, errors);
}

ArrayDeclarationStatement::~ArrayDeclarationStatement() {
}

const ExecutionResult ArrayDeclarationStatement::Execute(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	ErrorListRef errors = ErrorList::GetTerminator();
	if (GetInitializerExpression()) {
		const_shared_ptr<Result> initializer_result =
				GetInitializerExpression()->Evaluate(context, closure);
		errors = initializer_result->GetErrors();

		if (ErrorList::IsTerminator(errors)) {
			auto array = initializer_result->GetData<Array>();
			auto symbol_context = context;
			SetResult result = symbol_context->SetSymbol(*GetName(), array,
					context->GetTypeTable());
			errors =
					ToErrorListRef(result,
							GetInitializerExpression()->GetLocation(),
							GetName(),
							symbol_context->GetSymbol(GetName(), SHALLOW)->GetTypeSpecifier(),
							array->GetTypeSpecifier());
		}
	}

	return ExecutionResult(errors);
}

const_shared_ptr<TypeSpecifier> ArrayDeclarationStatement::GetTypeSpecifier() const {
	return m_type_specifier;
}

const DeclarationStatement* ArrayDeclarationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
	return new ArrayDeclarationStatement(GetLocation(), m_type_specifier,
			m_type_specifier_location, GetName(), GetNameLocation(), expression);
}

const yy::location ArrayDeclarationStatement::GetTypeSpecifierLocation() const {
	return m_type_specifier_location;
}
