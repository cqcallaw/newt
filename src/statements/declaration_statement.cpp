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

void DeclarationStatement::execute(
		const ExecutionContext* execution_context) const {

	const Symbol* symbol = Symbol::GetSymbol(m_type, m_name,
			m_initializer_expression, m_type_position, m_name_position,
			m_initializer_position);

	SymbolTable* symbol_table =
			(SymbolTable*) execution_context->GetSymbolTable();

	if (symbol != Symbol::DefaultSymbol) {
		InsertResult result = symbol_table->InsertSymbol(symbol);
		if (result == SYMBOL_EXISTS) {
			Error::semantic_error(Error::PREVIOUSLY_DECLARED_VARIABLE,
					m_name_position.first_line, m_name_position.first_column,
					*m_name);
		}
	}
}
