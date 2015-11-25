/*
 Copyright (C) 2015 The newt Authors.

 This file is part of newt.

 newt is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 newt is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with newt.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <execution_context.h>
#include <symbol_table.h>
#include <type_table.h>

ExecutionContext::ExecutionContext(SymbolContext* symbol_context,
		TypeTable* type_table) :
		m_symbol_context(symbol_context), m_type_table(type_table), m_return_value(
				nullptr) {
}

ExecutionContext::ExecutionContext() :
		m_symbol_context(new SymbolTable()), m_type_table(new TypeTable()), m_return_value(
				nullptr) {
}

ExecutionContext::~ExecutionContext() {
}

const ExecutionContext* ExecutionContext::GetDefault() {
	const static ExecutionContext* instance = new ExecutionContext(
			SymbolContext::GetDefault(), TypeTable::GetDefault());
	return instance;
}
