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
		const Expression* initializer_expression) :
		DeclarationStatement(position), m_type(type_specifier), m_type_position(
				type_position), m_name(name), m_name_position(name_position), m_initializer_expression(
				initializer_expression) {
}

const LinkedList<const Error*>* ArrayDeclarationStatement::preprocess(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::GetTerminator();

	const_shared_ptr<CompoundTypeSpecifier> element_type_as_compound =
			std::dynamic_pointer_cast<const CompoundTypeSpecifier>(
					m_type->GetElementTypeSpecifier());
	if (element_type_as_compound != nullptr) {
		//check that element type exists
		const_shared_ptr<TypeTable> type_table =
				execution_context->GetTypeTable();
		const string type_name = element_type_as_compound->GetTypeName();
		const CompoundType* type = type_table->GetType(type_name);

		if (type == CompoundType::GetDefaultCompoundType()) {
			errors = errors->With(
					new Error(Error::SEMANTIC, Error::UNDECLARED_TYPE,
							m_type_position.begin.line,
							m_type_position.begin.column, type_name));
		}
	}

	if (errors->IsTerminator()) {
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
						errors->With(
								new Error(Error::SEMANTIC,
										Error::ASSIGNMENT_TYPE_ERROR,
										m_initializer_expression->GetPosition().begin.line,
										m_initializer_expression->GetPosition().begin.column,
										m_type->ToString(),
										initializer_expression_type->ToString()));
			}
		}

		if (errors->IsTerminator()) {
			array = new Array(m_type->GetElementTypeSpecifier(),
					*execution_context->GetTypeTable());
		}

		SymbolTable* symbol_table =
				(SymbolTable*) execution_context->GetSymbolContext();

		if (array != nullptr) {
			auto wrapper = const_shared_ptr<Array>(array);
			const Symbol* symbol = new Symbol(wrapper);
			InsertResult insert_result = symbol_table->InsertSymbol(*name,
					symbol);
			if (insert_result == SYMBOL_EXISTS) {
				errors = errors->With(
						new Error(Error::SEMANTIC,
								Error::PREVIOUSLY_DECLARED_VARIABLE,
								m_name_position.begin.line,
								m_name_position.begin.column, *m_name));
			}
		}
	}

	return errors;
}

ArrayDeclarationStatement::~ArrayDeclarationStatement() {
}

const LinkedList<const Error*>* ArrayDeclarationStatement::execute(
		ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::GetTerminator();
	if (m_initializer_expression) {
		const Result* initializer_result = m_initializer_expression->Evaluate(
				execution_context);
		errors = initializer_result->GetErrors();

		if (errors->IsTerminator()) {
			auto array = static_pointer_cast<const Array>(
					initializer_result->GetData());
			auto symbol_context = execution_context->GetSymbolContext();
			SetResult result = symbol_context->SetSymbol(*m_name, array);
			errors = ToErrorList(result,
					m_initializer_expression->GetPosition(), m_name,
					symbol_context->GetSymbol(m_name, SHALLOW)->GetType(),
					array->GetTypeSpecifier());
		}

		delete (initializer_result);
	}

	return errors;
}

const_shared_ptr<TypeSpecifier> ArrayDeclarationStatement::GetType() const {
	return m_type;
}

const DeclarationStatement* ArrayDeclarationStatement::WithInitializerExpression(
		const Expression* expression) const {
	return new ArrayDeclarationStatement(GetPosition(), m_type, m_type_position,
			m_name, m_name_position, expression);
}
