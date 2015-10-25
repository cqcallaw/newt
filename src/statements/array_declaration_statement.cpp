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
#include <type_specifier.h>
#include <compound_type.h>
#include <array_type_specifier.h>
#include <dimension_list.h>

ArrayDeclarationStatement::ArrayDeclarationStatement(
		const ArrayTypeSpecifier* type_specifier, const YYLTYPE type_position,
		const std::string* name, const YYLTYPE name_position,
		const Expression* initializer_expression) :
		m_type(type_specifier), m_type_position(type_position), m_name(name), m_name_position(
				name_position), m_initializer_expression(initializer_expression) {
}

const LinkedList<const Error*>* ArrayDeclarationStatement::preprocess(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	const CompoundTypeSpecifier* element_type_as_compound =
			dynamic_cast<const CompoundTypeSpecifier*>(m_type->GetElementTypeSpecifier());
	if (element_type_as_compound != nullptr) {
		//check that element type exists
		const TypeTable* type_table = execution_context->GetTypeTable();
		const string type_name = element_type_as_compound->GetTypeName();
		const CompoundType* type = type_table->GetType(type_name);

		if (type == CompoundType::GetDefaultCompoundType()) {
			errors = errors->With(
					new Error(Error::SEMANTIC, Error::UNDECLARED_TYPE,
							m_type_position.first_line,
							m_type_position.first_column, type_name));
		}
	}

	if (errors->IsTerminator()) {
		const Array* array = nullptr;
		const string* name = m_name;
		if (m_initializer_expression) {
			const TypeSpecifier* initializer_expression_type =
					m_initializer_expression->GetType(execution_context);
			const ArrayTypeSpecifier* as_array =
					dynamic_cast<const ArrayTypeSpecifier*>(initializer_expression_type);
			if (!as_array
					|| !initializer_expression_type->IsAssignableTo(m_type)) {
				errors =
						errors->With(
								new Error(Error::SEMANTIC,
										Error::ASSIGNMENT_TYPE_ERROR,
										m_initializer_expression->GetPosition().first_line,
										m_initializer_expression->GetPosition().first_column,
										m_type->ToString(),
										initializer_expression_type->ToString()));
			}
		}

		if (errors->IsTerminator()) {
			array = new Array(m_type->GetElementTypeSpecifier(),
					execution_context->GetTypeTable());
		}

		SymbolTable* symbol_table =
				(SymbolTable*) execution_context->GetSymbolContext();

		if (array != nullptr) {
			const Symbol* symbol = new Symbol(name, array);
			InsertResult insert_result = symbol_table->InsertSymbol(symbol);
			if (insert_result == SYMBOL_EXISTS) {
				errors = errors->With(
						new Error(Error::SEMANTIC,
								Error::PREVIOUSLY_DECLARED_VARIABLE,
								m_name_position.first_line,
								m_name_position.first_column, *m_name));
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
			LinkedList<const Error*>::Terminator;
	if (m_initializer_expression) {
		const Result* initializer_result = m_initializer_expression->Evaluate(
				execution_context);
		errors = initializer_result->GetErrors();

		if (errors->IsTerminator()) {
			const Array* array =
					static_cast<const Array*>(initializer_result->GetData());
			SetResult result = execution_context->GetSymbolContext()->SetSymbol(
					*m_name, array);
			errors = ToErrorList(result,
					m_initializer_expression->GetPosition(), m_name);
		}

		delete (initializer_result);
	}

	return errors;
}

const TypeSpecifier* ArrayDeclarationStatement::GetType() const {
	return m_type;
}

const DeclarationStatement* ArrayDeclarationStatement::WithInitializerExpression(
		const Expression* expression) const {
	return new ArrayDeclarationStatement(m_type, m_type_position, m_name,
			m_name_position, expression);
}

const std::string* ArrayDeclarationStatement::GetName() const {
	return m_name;
}
