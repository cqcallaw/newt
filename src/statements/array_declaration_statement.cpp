/*
 * array_declaration_statement.cpp
 *
 *  Created on: Jul 28, 2015
 *      Author: caleb
 */

#include <array_declaration_statement.h>
#include <error.h>
#include <symbol_table.h>
#include <execution_context.h>
#include <expression.h>
#include <assignment_statement.h>
#include <variable.h>
#include <array_symbol.h>
#include <sstream>
#include <constant_expression.h>
#include <vector>
#include <type_specifier.h>

ArrayDeclarationStatement::ArrayDeclarationStatement(
		const ArrayTypeSpecifier* element_type_specifier,
		const YYLTYPE type_position, const std::string* name,
		const YYLTYPE name_position, const Expression* size_expression,
		const YYLTYPE size_expression_position) :
		m_type(element_type_specifier), m_type_position(type_position), m_name(
				name), m_name_position(name_position), m_size_expression(
				size_expression), m_size_expression_position(
				size_expression_position) {
}

const LinkedList<const Error*>* ArrayDeclarationStatement::preprocess(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* result =
			LinkedList<const Error*>::Terminator;
	const Symbol* symbol = (Symbol*) Symbol::DefaultSymbol;
	const string* name = m_name;

	int size = 0;
	//if our array size is a constant, validate it as part of the preprocessing pass.
	//array sizes that are variable are processed at runtime.
	if (IsFixedSize()) {
		const TypeSpecifier* size_expression_type = m_size_expression->GetType(
				execution_context);

		const PrimitiveTypeSpecifier* size_type_as_primitive =
				dynamic_cast<const PrimitiveTypeSpecifier*>(size_expression_type);
		if (size_type_as_primitive == nullptr
				|| !size_type_as_primitive->IsAssignableTo(
						PrimitiveTypeSpecifier::INT)) {
			result = result->With(
					new Error(Error::SEMANTIC, Error::INVALID_ARRAY_SIZE_TYPE,
							m_size_expression_position.first_line,
							m_size_expression_position.first_column,
							size_expression_type->ToString(), *m_name));
			return result;
		} else if (m_size_expression->IsConstant()) {
			const Result* evaluation = m_size_expression->Evaluate(
					execution_context);
			const LinkedList<const Error*>* evaluation_errors =
					evaluation->GetErrors();

			if (evaluation_errors != LinkedList<const Error*>::Terminator) {
				result = result->Concatenate(evaluation_errors, true);
			}

			if (evaluation != NULL) {
				int array_size = *((int*) (evaluation)->GetData());

				if (array_size <= 0) {
					ostringstream convert;
					convert << array_size;
					result = result->With(
							new Error(Error::SEMANTIC,
									Error::INVALID_ARRAY_SIZE,
									m_size_expression_position.first_line,
									m_size_expression_position.first_column,
									*m_name, convert.str()));

					return result;
				} else {
					size = array_size;
				}
			}

			delete (evaluation);

		}
		symbol = new ArraySymbol(*name, m_type->GetElementTypeSpecifier(),
				execution_context->GetTypeTable(), size,
				m_size_expression->IsConstant());
	} else {
		symbol = new ArraySymbol(*name, m_type->GetElementTypeSpecifier(),
				execution_context->GetTypeTable());
	}

	SymbolTable* symbol_table =
			(SymbolTable*) execution_context->GetSymbolContext();

	if (symbol != Symbol::DefaultSymbol) {
		InsertResult insert_result = symbol_table->InsertSymbol(symbol);
		if (insert_result == SYMBOL_EXISTS) {
			result = result->With(
					new Error(Error::SEMANTIC,
							Error::PREVIOUSLY_DECLARED_VARIABLE,
							m_name_position.first_line,
							m_name_position.first_column, *m_name));
		}
	}

	return result;
}

ArrayDeclarationStatement::~ArrayDeclarationStatement() {
}

const LinkedList<const Error*>* ArrayDeclarationStatement::execute(
		const ExecutionContext* execution_context) const {
	LinkedList<const Error*>* errors = LinkedList<const Error*>::Terminator;
	if (IsFixedSize() && !m_size_expression->IsConstant()) {
		//handle non-constant but fixed array sizes
		const void* value = nullptr;

		SymbolTable* symbol_table =
				(SymbolTable*) execution_context->GetSymbolContext();

		const TypeSpecifier* size_expression_type = m_size_expression->GetType(
				execution_context);
		const PrimitiveTypeSpecifier* size_type_as_primitive =
				dynamic_cast<const PrimitiveTypeSpecifier*>(size_expression_type);

		if (size_type_as_primitive == nullptr
				|| size_type_as_primitive->GetBasicType() != INT) {
			errors = (LinkedList<const Error*>*) errors->With(
					new Error(Error::SEMANTIC, Error::INVALID_ARRAY_SIZE_TYPE,
							m_size_expression_position.first_line,
							m_size_expression_position.first_column,
							size_expression_type->ToString(), *m_name));
		} else {
			SetResult result;

			const Result* evaluation = m_size_expression->Evaluate(
					execution_context);
			auto evaluation_errors = evaluation->GetErrors();
			if (evaluation_errors != LinkedList<const Error*>::Terminator) {
				errors = (LinkedList<const Error*>*) evaluation_errors;
			} else {
				value = evaluation->GetData();
				if (value != NULL) {
					int array_size = *((int*) (value));

					if (array_size <= 0) {
						ostringstream convert;
						convert << array_size;
						errors = (LinkedList<const Error*>*) errors->With(
								new Error(Error::RUNTIME,
										Error::INVALID_ARRAY_SIZE,
										m_size_expression_position.first_line,
										m_size_expression_position.first_column,
										*m_name, convert.str()));
					} else {
						const ArraySymbol* symbol = new ArraySymbol(*m_name,
								m_type->GetElementTypeSpecifier(),
								execution_context->GetTypeTable(), array_size,
								true);
						result = symbol_table->SetArraySymbol(*m_name, symbol);
					}
				} else {
					assert(false);
				}
			}

			delete (evaluation);

			if (result != SET_SUCCESS) {
				assert(false);
			}
		}
	}

	return errors;
}

const bool ArrayDeclarationStatement::IsFixedSize() const {
	return m_size_expression != nullptr;
}
