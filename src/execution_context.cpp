/*
 * execution_context.cpp
 *
 *  Created on: Jun 20, 2015
 *      Author: caleb
 */

#include <execution_context.h>
#include <symbol_table.h>
#include <type_table.h>

ExecutionContext::ExecutionContext(SymbolContext* symbol_context,
		TypeTable* type_table) :
		m_symbol_context(symbol_context), m_type_table(type_table) {
}

ExecutionContext::ExecutionContext() :
		m_symbol_context(new SymbolTable()), m_type_table(new TypeTable()) {
}

ExecutionContext::~ExecutionContext() {
}

