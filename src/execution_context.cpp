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

ExecutionContext::ExecutionContext() :
		ExecutionContext(Modifier::Type::NONE, make_shared<symbol_map>(),
				SymbolContextList::GetTerminator(), make_shared<TypeTable>(),
				plain_shared_ptr<void>(nullptr), plain_shared_ptr<int>(nullptr),
				PERSISTENT) {
}

ExecutionContext::ExecutionContext(const shared_ptr<SymbolContext> existing,
		volatile_shared_ptr<TypeTable> type_table, const LifeTime life_time) :
		ExecutionContext(existing->GetModifiers(), existing->GetTable(),
				existing->GetParent(), type_table,
				plain_shared_ptr<void>(nullptr), plain_shared_ptr<int>(nullptr),
				life_time) {
}

ExecutionContext::ExecutionContext(const Modifier::Type modifiers,
		const SymbolContextListRef parent_context,
		volatile_shared_ptr<TypeTable> type_table, const LifeTime life_time) :
		ExecutionContext(modifiers, make_shared<symbol_map>(), parent_context,
				type_table, plain_shared_ptr<void>(nullptr),
				plain_shared_ptr<int>(nullptr), life_time) {
}

ExecutionContext::ExecutionContext(const Modifier::Type modifiers,
		const shared_ptr<symbol_map> symbol_map,
		const SymbolContextListRef parent_context,
		volatile_shared_ptr<TypeTable> type_table,
		plain_shared_ptr<void> m_return_value, plain_shared_ptr<int> exit_code,
		const LifeTime life_time) :
		SymbolTable(modifiers, parent_context, symbol_map), m_type_table(
				type_table), m_return_value(m_return_value), m_exit_code(
				exit_code), m_life_time(life_time) {
}

ExecutionContext::~ExecutionContext() {
}

const shared_ptr<ExecutionContext> ExecutionContext::GetDefault() {
	static const shared_ptr<ExecutionContext> instance = make_shared<
			ExecutionContext>(Modifier::READONLY,
			SymbolContextList::GetTerminator(), TypeTable::GetDefault(),
			PERSISTENT);
	return instance;
}
