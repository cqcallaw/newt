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

	int size = 0;
	bool initialized = false;

	//if our array size is a constant, validate it as part of the preprocessing pass.
	//array sizes that are variable are processed at runtime.
	if (m_size_expression != NULL) {
		const Type size_expression_type = m_size_expression->GetType(
				execution_context);
		if (size_expression_type != INT) {
			result = (LinkedList<const Error*>*) result->With(
					new Error(Error::SEMANTIC, Error::INVALID_ARRAY_SIZE_TYPE,
							m_size_expression_position.first_line,
							m_size_expression_position.first_column,
							type_to_string(size_expression_type), *m_name));
			return result;
		} else if (m_size_expression->IsConstant()) {
			const Result* evaluation = m_size_expression->Evaluate(
					execution_context);
			const LinkedList<const Error*>* evaluation_errors =
					evaluation->GetErrors();

			if (evaluation_errors != LinkedList<const Error*>::Terminator) {
				result = (LinkedList<const Error*>*) result->Concatenate(
						evaluation_errors, true);
			}

			if (evaluation != NULL) {
				int array_size = *((int*) (evaluation)->GetData());

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
				} else {
					//our array size is a constant, so we can allocate memory now instead of in the execution pass
					size = array_size;
					initialized = true;
				}
			}

			delete (evaluation);
		}
	}

	switch (m_type) {
	case INT:
		symbol = new ArraySymbol(name, new int[0](), size, initialized);
		break;
	case DOUBLE:
		symbol = new ArraySymbol(name, new double[0](), size, initialized);
		break;
	case STRING:
		symbol = new ArraySymbol(name, (const string**) new string*[0](), size,
				initialized);
		break;
	default:
		result = (LinkedList<const Error*>*) result->With(
				new Error(Error::SEMANTIC, Error::INVALID_ARRAY_TYPE,
						m_type_position.first_line,
						m_type_position.first_column, *name));
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

const LinkedList<const Error*>* ArrayDeclarationStatement::execute(
		const ExecutionContext* execution_context) const {
	LinkedList<const Error*>* errors = LinkedList<const Error*>::Terminator;
	const void* value = nullptr;

	SymbolTable* symbol_table =
			(SymbolTable*) execution_context->GetSymbolTable();

	if (m_size_expression->GetType(execution_context) != INT) {
		errors = (LinkedList<const Error*>*) errors->With(
				new Error(Error::SEMANTIC, Error::INVALID_ARRAY_SIZE_TYPE,
						m_size_expression_position.first_line,
						m_size_expression_position.first_column, *m_name));
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
							new Error(Error::SEMANTIC,
									Error::INVALID_ARRAY_SIZE,
									m_size_expression_position.first_line,
									m_size_expression_position.first_column,
									*m_name, convert.str()));
				} else {
					switch (m_type) {
					case INT: {
						int* array = new int[array_size]();

						for (int i = 0; i < array_size; i++) {
							array[i] = 0;
						}

						//symbol = new ArraySymbol(m_name, array, array_size);
						result = symbol_table->SetArraySymbol(*m_name, array,
								array_size, true);
						break;
					}
					case DOUBLE: {
						double* array = new double[array_size]();

						for (int i = 0; i < array_size; i++) {
							array[i] = 0.0;
						}

						result = symbol_table->SetArraySymbol(*m_name, array,
								array_size, true);
						break;
					}
					case STRING: {
						const string** array =
								(const string**) new string*[array_size]();

						for (int i = 0; i < array_size; i++) {
							array[i] = new string("");
						}

						result = symbol_table->SetArraySymbol(*m_name, array,
								array_size, true);
						break;
					}
					default:
						break;
					}
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

	return errors;
}
