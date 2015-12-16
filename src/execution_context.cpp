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
#include <memory>

ExecutionContext::ExecutionContext(SymbolContext* symbol_context,
		TypeTable* type_table, bool dispose_members) :
		ExecutionContext(symbol_context, type_table, nullptr, dispose_members) {
}

ExecutionContext::ExecutionContext() :
		ExecutionContext(new SymbolTable(), new TypeTable(), nullptr, true) {
}

ExecutionContext::ExecutionContext(SymbolContext* symbol_context,
		TypeTable* type_table, const void* m_return_value, bool dispose_members) :
		m_symbol_context(symbol_context), m_type_table(type_table), m_return_value(
				m_return_value), m_dispose_members(dispose_members) {
}

ExecutionContext::~ExecutionContext() {
	if (m_dispose_members) {
		delete m_symbol_context;
		delete m_type_table;
	}
}

const ExecutionContext* ExecutionContext::GetDefault() {
	const static std::unique_ptr<ExecutionContext> instance = std::unique_ptr
			< ExecutionContext
			> (new ExecutionContext(SymbolContext::GetDefault(),
					TypeTable::GetDefault()));
	return instance.get();
}
