/*
 * array_declaration_statement.cpp
 *
 *  Created on: Jul 28, 2015
 *      Author: caleb
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

ArrayDeclarationStatement::ArrayDeclarationStatement(
		const yy::location position,
		const_shared_ptr<ArrayTypeSpecifier> type_specifier,
		const yy::location type_position, const_shared_ptr<string> name,
		const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) :
		DeclarationStatement(position, name, name_position,
				initializer_expression, ModifierList::GetTerminator(),
				GetDefaultLocation()), m_type(type_specifier), m_type_position(
				type_position) {
}

const ErrorListRef ArrayDeclarationStatement::preprocess(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	const_shared_ptr<ComplexTypeSpecifier> element_type_as_record =
			dynamic_pointer_cast<const ComplexTypeSpecifier>(
					m_type->GetElementTypeSpecifier());
	if (element_type_as_record) {
		//check that element type exists
		const_shared_ptr<TypeTable> type_table =
				execution_context->GetTypeTable();
		const string type_name = *element_type_as_record->GetTypeName();
		const_shared_ptr<ComplexType> type = type_table->GetType<ComplexType>(
				type_name);

		if (!type) {
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC, Error::UNDECLARED_TYPE,
							m_type_position.begin.line,
							m_type_position.begin.column, type_name), errors);
		}
	}

	if (ErrorList::IsTerminator(errors)) {
		const Array* array = nullptr;
		auto name = GetName();
		auto initializer_expression = GetInitializerExpression();
		if (initializer_expression) {
			const_shared_ptr<TypeSpecifier> initializer_expression_type =
					initializer_expression->GetTypeSpecifier(execution_context);
			const_shared_ptr<ArrayTypeSpecifier> as_array =
					std::dynamic_pointer_cast<const ArrayTypeSpecifier>(
							initializer_expression_type);
			if (!as_array
					|| !initializer_expression_type->IsAssignableTo(m_type,
							execution_context->GetTypeTable())) {
				errors =
						ErrorList::From(
								make_shared<Error>(Error::SEMANTIC,
										Error::ASSIGNMENT_TYPE_ERROR,
										initializer_expression->GetPosition().begin.line,
										initializer_expression->GetPosition().begin.column,
										m_type->ToString(),
										initializer_expression_type->ToString()),
								errors);
			}
		}

		if (ErrorList::IsTerminator(errors)) {
			array = new Array(m_type->GetElementTypeSpecifier(),
					*execution_context->GetTypeTable());
		}

		volatile_shared_ptr<SymbolTable> symbol_table = static_pointer_cast<
				SymbolTable>(execution_context);

		if (array) {
			auto wrapper = const_shared_ptr<Array>(array);
			auto symbol = const_shared_ptr<Symbol>(new Symbol(wrapper));
			InsertResult insert_result = symbol_table->InsertSymbol(*name,
					symbol);
			if (insert_result == SYMBOL_EXISTS) {
				errors = ErrorList::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::PREVIOUS_DECLARATION,
								GetNamePosition().begin.line,
								GetNamePosition().begin.column, *GetName()),
						errors);
			}
		}
	}

	return errors;
}

ArrayDeclarationStatement::~ArrayDeclarationStatement() {
}

const ErrorListRef ArrayDeclarationStatement::execute(
		shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();
	if (GetInitializerExpression()) {
		const_shared_ptr<Result> initializer_result =
				GetInitializerExpression()->Evaluate(execution_context);
		errors = initializer_result->GetErrors();

		if (ErrorList::IsTerminator(errors)) {
			auto array = initializer_result->GetData<Array>();
			auto symbol_context = execution_context;
			SetResult result = symbol_context->SetSymbol(*GetName(), array,
					execution_context->GetTypeTable());
			errors = ToErrorListRef(result,
					GetInitializerExpression()->GetPosition(), GetName(),
					symbol_context->GetSymbol(GetName(), SHALLOW)->GetType(),
					array->GetTypeSpecifier());
		}
	}

	return errors;
}

const_shared_ptr<TypeSpecifier> ArrayDeclarationStatement::GetType() const {
	return m_type;
}

const DeclarationStatement* ArrayDeclarationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
	return new ArrayDeclarationStatement(GetPosition(), m_type, m_type_position,
			GetName(), GetNamePosition(), expression);
}
