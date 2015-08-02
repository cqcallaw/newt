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

ArrayDeclarationStatement::ArrayDeclarationStatement(const Type type,
		const YYLTYPE type_position, const std::string* name,
		const YYLTYPE name_position, const Expression* size_expression,
		const YYLTYPE size_expression_position) :
		m_type(type), m_type_position(type_position), m_name(name), m_name_position(
				name_position), m_size_expression(size_expression), m_size_expression_position(
				size_expression_position) {
}

LinkedList<const Error*>* ArrayDeclarationStatement::preprocess(
		const ExecutionContext* execution_context) const {
	LinkedList<const Error*>* result = LinkedList<const Error*>::Terminator;
	Symbol* symbol = (Symbol*) Symbol::DefaultSymbol;
	const string* name = m_name;

	switch (m_type) {
	case INT:
		symbol = new ArraySymbol(name, new int[0](), 0);
		break;
	case DOUBLE:
		symbol = new ArraySymbol(name, new double[0](), 0);
		break;
	case STRING:
		symbol = new ArraySymbol(name, (const string**) new string*[0](), 0);
		break;
	default:
		result = (LinkedList<const Error*>*) result->With(
				new Error(Error::SEMANTIC, Error::INVALID_ARRAY_TYPE,
						m_type_position.first_line,
						m_type_position.first_column, *name));
	}

	//if our array size is a constant, validate it as part of the preprocessing pass.
	//array sizes that are variable are processed at runtime.
	if (m_size_expression != NULL && m_size_expression->IsConstant()) {
		if (m_size_expression->GetType(execution_context) != INT) {
			result = (LinkedList<const Error*>*) result->With(
					new Error(Error::SEMANTIC, Error::INVALID_ARRAY_SIZE,
							m_size_expression_position.first_line,
							m_size_expression_position.first_column, *m_name,
							*(m_size_expression->ToString(execution_context))));
			return result;
		} else {
			const void* evaluation = m_size_expression->Evaluate(
					execution_context);
			if (evaluation != NULL) {
				int array_size = *((int*) (evaluation));

				if (array_size <= 0) {
					ostringstream convert;
					convert << array_size;
					result = (LinkedList<const Error*>*) result->With(
							new Error(Error::SEMANTIC,
									Error::INVALID_ARRAY_SIZE,
									m_size_expression_position.first_line,
									m_size_expression_position.first_column,
									*m_name, convert.str()));

					return result;
				}
			}
		}
	}

	SymbolTable* symbol_table =
			(SymbolTable*) execution_context->GetSymbolTable();

	if (symbol != Symbol::DefaultSymbol) {
		InsertResult insert_result = symbol_table->InsertSymbol(symbol);
		if (insert_result == SYMBOL_EXISTS) {
			result = (LinkedList<const Error*>*) result->With(
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

void ArrayDeclarationStatement::execute(
		const ExecutionContext* execution_context) const {
	SymbolTable* symbol_table =
			(SymbolTable*) execution_context->GetSymbolTable();

	if (m_size_expression->GetType(execution_context) != INT) {
		Error::semantic_error(Error::INVALID_ARRAY_SIZE,
				m_size_expression_position.first_line,
				m_size_expression_position.first_column, *m_name,
				*(m_size_expression->ToString(execution_context)));
	} else {
		SetResult result;

		const void* evaluation = m_size_expression->Evaluate(execution_context);
		if (evaluation != NULL) {
			int array_size = *((int*) (evaluation));

			if (array_size <= 0) {
				ostringstream convert;
				convert << array_size;
				Error::semantic_error(Error::INVALID_ARRAY_SIZE,
						m_size_expression_position.first_line,
						m_size_expression_position.first_column, *m_name,
						convert.str());

				return;
			} else {
				switch (m_type) {
				case INT: {
					int* array = new int[array_size]();

					for (int i = 0; i < array_size; i++) {
						array[i] = 0;
					}

					//symbol = new ArraySymbol(m_name, array, array_size);
					result = symbol_table->SetArraySymbol(*m_name, array,
							array_size);
					break;
				}
				case DOUBLE: {
					double* array = new double[array_size]();

					for (int i = 0; i < array_size; i++) {
						array[i] = 0.0;
					}

					result = symbol_table->SetArraySymbol(*m_name, array,
							array_size);
					break;
				}
				case STRING: {
					const string** array =
							(const string**) new string*[array_size]();

					for (int i = 0; i < array_size; i++) {
						array[i] = new string("");
					}

					result = symbol_table->SetArraySymbol(*m_name, array,
							array_size);
					break;
				}
				default:
					break;
				}
			}
		} else {
			assert(false);
		}

		if (result != SET_SUCCESS) {
			assert(false);
		}
	}
}
