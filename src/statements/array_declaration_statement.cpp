/*
 * array_declaration_statement.cpp
 *
 *  Created on: Jul 28, 2015
 *      Author: caleb
 */

#include <array.h>
#include <array_declaration_statement.h>
#include <error.h>
#include <symbol_table.h>
#include <execution_context.h>
#include <expression.h>
#include <assignment_statement.h>
#include <variable.h>
#include <sstream>
#include <constant_expression.h>
#include <vector>
#include <compound_type.h>
#include <array_type_specifier.h>
#include <type_specifier.h>

ArrayDeclarationStatement::ArrayDeclarationStatement(
		const yy::location position,
		const_shared_ptr<ArrayTypeSpecifier> type_specifier,
		const yy::location type_position, const_shared_ptr<string> name,
		const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) :
		DeclarationStatement(position), m_type(type_specifier), m_type_position(
				type_position), m_name(name), m_name_position(name_position), m_initializer_expression(
				initializer_expression) {
}

const ErrorList ArrayDeclarationStatement::preprocess(
		const_shared_ptr<ExecutionContext> execution_context) const {
	ErrorList errors = ErrorListBase::GetTerminator();

	const_shared_ptr<CompoundTypeSpecifier> element_type_as_compound =
			std::dynamic_pointer_cast<const CompoundTypeSpecifier>(
					m_type->GetElementTypeSpecifier());
	if (element_type_as_compound != nullptr) {
		//check that element type exists
		const_shared_ptr<TypeTable> type_table =
				execution_context->GetTypeTable();
		const string type_name = element_type_as_compound->GetTypeName();
		const_shared_ptr<CompoundType> type = type_table->GetType(type_name);

		if (type == CompoundType::GetDefaultCompoundType()) {
			errors = ErrorListBase::From(
					make_shared<Error>(Error::SEMANTIC, Error::UNDECLARED_TYPE,
							m_type_position.begin.line,
							m_type_position.begin.column, type_name), errors);
		}
	}

	if (ErrorListBase::IsTerminator(errors)) {
		const Array* array = nullptr;
		auto name = m_name;
		if (m_initializer_expression) {
			const_shared_ptr<TypeSpecifier> initializer_expression_type =
					m_initializer_expression->GetType(execution_context);
			const_shared_ptr<ArrayTypeSpecifier> as_array =
					std::dynamic_pointer_cast<const ArrayTypeSpecifier>(
							initializer_expression_type);
			if (!as_array
					|| !initializer_expression_type->IsAssignableTo(m_type)) {
				errors =
						ErrorListBase::From(
								make_shared<Error>(Error::SEMANTIC,
										Error::ASSIGNMENT_TYPE_ERROR,
										m_initializer_expression->GetPosition().begin.line,
										m_initializer_expression->GetPosition().begin.column,
										m_type->ToString(),
										initializer_expression_type->ToString()),
								errors);
			}
		}

		if (ErrorListBase::IsTerminator(errors)) {
			array = new Array(m_type->GetElementTypeSpecifier(),
					*execution_context->GetTypeTable());
		}

		volatile_shared_ptr<SymbolTable> symbol_table = static_pointer_cast<
				SymbolTable>(execution_context->GetSymbolContext());

		if (array != nullptr) {
			auto wrapper = const_shared_ptr<Array>(array);
			auto symbol = const_shared_ptr<Symbol>(new Symbol(wrapper));
			InsertResult insert_result = symbol_table->InsertSymbol(*name,
					symbol);
			if (insert_result == SYMBOL_EXISTS) {
				errors = ErrorListBase::From(
						make_shared<Error>(Error::SEMANTIC,
								Error::PREVIOUSLY_DECLARED_VARIABLE,
								m_name_position.begin.line,
								m_name_position.begin.column, *m_name), errors);
			}
		}
	}

	return errors;
}

ArrayDeclarationStatement::~ArrayDeclarationStatement() {
}

const ErrorList ArrayDeclarationStatement::execute(
		shared_ptr<ExecutionContext> execution_context) const {
	ErrorList errors = ErrorListBase::GetTerminator();
	if (m_initializer_expression) {
		const_shared_ptr<Result> initializer_result =
				m_initializer_expression->Evaluate(execution_context);
		errors = initializer_result->GetErrors();

		if (ErrorListBase::IsTerminator(errors)) {
			auto array = static_pointer_cast<const Array>(
					initializer_result->GetData());
			auto symbol_context = execution_context->GetSymbolContext();
			SetResult result = symbol_context->SetSymbol(*m_name, array);
			errors = ToErrorList(result,
					m_initializer_expression->GetPosition(), m_name,
					symbol_context->GetSymbol(m_name, SHALLOW)->GetType(),
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
			m_name, m_name_position, expression);
}
