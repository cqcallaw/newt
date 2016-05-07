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
				Symbol::GetDefaultSymbol(), plain_shared_ptr<int>(nullptr),
				PERSISTENT) {
}

ExecutionContext::ExecutionContext(const Modifier::Type modifiers) :
		ExecutionContext(modifiers, make_shared<symbol_map>(),
				SymbolContextList::GetTerminator(), make_shared<TypeTable>(),
				Symbol::GetDefaultSymbol(), plain_shared_ptr<int>(nullptr),
				PERSISTENT) {
}
ExecutionContext::ExecutionContext(const shared_ptr<SymbolContext> existing,
		volatile_shared_ptr<TypeTable> type_table, const LifeTime life_time) :
		ExecutionContext(existing, m_parent, type_table,
				Symbol::GetDefaultSymbol(), plain_shared_ptr<int>(nullptr),
				life_time) {
}

ExecutionContext::ExecutionContext(const Modifier::Type modifiers,
		const SymbolContextListRef parent_context,
		volatile_shared_ptr<TypeTable> type_table, const LifeTime life_time) :
		ExecutionContext(modifiers, make_shared<symbol_map>(), parent_context,
				type_table, Symbol::GetDefaultSymbol(),
				plain_shared_ptr<int>(nullptr), life_time) {
}

const_shared_ptr<Symbol> ExecutionContext::GetSymbol(const string& identifier,
		const SearchType search_type) const {
	auto result = SymbolContext::GetSymbol(identifier);

	if (result == Symbol::GetDefaultSymbol() && m_parent
			&& search_type == DEEP) {
		return m_parent->GetData()->GetSymbol(identifier, search_type);
	} else {
		return result;
	}
}

const void ExecutionContext::print(ostream& os, const TypeTable& type_table,
		const Indent& indent, const SearchType search_type) const {
	SymbolContext::print(os, type_table, indent);

	if (search_type == DEEP && m_parent) {
		os << indent + 1 << "-->" << m_parent->GetData() << "("
				<< (m_parent->IsWeak() ? string("weak") : string("strong"))
				<< ")" << endl;
		m_parent->GetData()->print(os, type_table, indent + 1, search_type);
	}
}

SetResult ExecutionContext::SetSymbol(const string& identifier,
		const_shared_ptr<TypeSpecifier> type, const_shared_ptr<void> value,
		const TypeTable& type_table) {
	auto result = SymbolContext::SetSymbol(identifier, type, value, type_table);

	if (result == UNDEFINED_SYMBOL && m_parent) {
		auto context = m_parent->GetData();
		return context->SetSymbol(identifier, type, value, type_table);
	} else {
		return result;
	}
}

const_shared_ptr<Symbol> ExecutionContext::GetSymbol(
		const_shared_ptr<string> identifier,
		const SearchType search_type) const {
	return GetSymbol(*identifier, search_type);
}

const shared_ptr<ExecutionContext> ExecutionContext::WithContents(
		const shared_ptr<SymbolContext> contents) const {
	return shared_ptr<ExecutionContext>(
			new ExecutionContext(contents, m_parent, m_type_table,
					m_return_value, m_exit_code, m_life_time));
}

ExecutionContext::ExecutionContext(const shared_ptr<SymbolContext> context,
		const SymbolContextListRef parent_context,
		volatile_shared_ptr<TypeTable> type_table,
		const_shared_ptr<Symbol> return_value, const_shared_ptr<int> exit_code,
		const LifeTime life_time) :
		SymbolTable(*context), m_parent(parent_context), m_type_table(
				type_table), m_return_value(return_value), m_exit_code(
				exit_code), m_life_time(life_time) {
}

ExecutionContext::ExecutionContext(const Modifier::Type modifiers,
		const shared_ptr<symbol_map> symbol_map,
		const SymbolContextListRef parent_context,
		volatile_shared_ptr<TypeTable> type_table,
		const_shared_ptr<Symbol> return_value, const_shared_ptr<int> exit_code,
		const LifeTime life_time) :
		SymbolTable(modifiers, symbol_map), m_parent(parent_context), m_type_table(
				type_table), m_return_value(return_value), m_exit_code(
				exit_code), m_life_time(life_time) {
}

ExecutionContext::~ExecutionContext() {
}

const shared_ptr<ExecutionContext> ExecutionContext::GetDefault() {
	static const shared_ptr<ExecutionContext> instance = make_shared<
			ExecutionContext>(Modifier::NONE,
			SymbolContextList::GetTerminator(), TypeTable::GetDefault(),
			PERSISTENT);
	return instance;
}
