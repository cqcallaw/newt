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

ExecutionContext::ExecutionContext(
		volatile_shared_ptr<SymbolContext> symbol_context,
		volatile_shared_ptr<TypeTable> type_table) :
		ExecutionContext(symbol_context, type_table,
				plain_shared_ptr<void>(nullptr), plain_shared_ptr<int>(nullptr)) {
}

ExecutionContext::ExecutionContext() :
		ExecutionContext(make_shared<SymbolTable>(), make_shared<TypeTable>(),
				plain_shared_ptr<void>(nullptr), plain_shared_ptr<int>(nullptr)) {
}

ExecutionContext::ExecutionContext(
		volatile_shared_ptr<SymbolContext> symbol_context,
		volatile_shared_ptr<TypeTable> type_table,
		plain_shared_ptr<void> m_return_value, plain_shared_ptr<int> exit_code) :
		m_symbol_context(symbol_context), m_type_table(type_table), m_return_value(
				m_return_value), m_exit_code(exit_code) {
}

ExecutionContext::~ExecutionContext() {
}

const_shared_ptr<ExecutionContext> ExecutionContext::GetDefault() {
	const static const_shared_ptr<ExecutionContext> instance = const_shared_ptr<
			ExecutionContext>(
			new ExecutionContext(SymbolContext::GetDefault(),
					TypeTable::GetDefault()));
	return instance;
}
