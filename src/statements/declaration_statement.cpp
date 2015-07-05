/*
 * declaration_statement.cpp
 *
 *  Created on: Jun 14, 2015
 *      Author: caleb
 */

#include <declaration_statement.h>
#include <error.h>
#include <symbol_table.h>
#include <execution_context.h>

DeclarationStatement::DeclarationStatement(const Type type,
		const YYLTYPE type_position, const std::string* name,
		const YYLTYPE name_position, const Expression* initializer_expression,
		const YYLTYPE initializer_position) :
		m_type(type), m_type_position(type_position), m_name(name), m_name_position(
				name_position), m_initializer_expression(
				initializer_expression), m_initializer_position(
				initializer_position) {
}

DeclarationStatement::~DeclarationStatement() {
}

LinkedList<Error*> DeclarationStatement::preprocess(
		const ExecutionContext* execution_context) {
	LinkedList<Error*> result = LinkedList<Error*>::Terminator;
	const Symbol* symbol = Symbol::GetSymbol(m_type, m_name,
			m_initializer_expression, m_type_position, m_name_position,
			m_initializer_position, execution_context);

	SymbolTable* symbol_table =
			(SymbolTable*) execution_context->GetSymbolTable();

	if (symbol != Symbol::DefaultSymbol) {
		InsertResult result = symbol_table->InsertSymbol(symbol);
		if (result == SYMBOL_EXISTS) {
			const Error* error = new Error(Error::PREVIOUSLY_DECLARED_VARIABLE,
					m_name_position.first_line, m_name_position.first_column,
					*m_name);
			result = new LinkedList<Error*>(error, result);
		}
	}

	return result;
}

void DeclarationStatement::execute(
		const ExecutionContext* execution_context) const {
}
