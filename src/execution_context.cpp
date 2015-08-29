/*
 * execution_context.cpp
 *
 *  Created on: Jun 20, 2015
 *      Author: caleb
 */

#include <execution_context.h>
#include <symbol_table.h>
#include <type_table.h>

ExecutionContext::ExecutionContext(SymbolTable* symbol_table,
		TypeTable* type_table) :
		m_symbol_table(symbol_table), m_type_table(type_table) {
}

ExecutionContext::ExecutionContext() :
		m_symbol_table(new SymbolTable()), m_type_table(new TypeTable()) {
}

ExecutionContext::~ExecutionContext() {
}

