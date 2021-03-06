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
				ExecutionContextList::GetTerminator(), make_shared<TypeTable>(),
				PERSISTENT, 0) {
}

ExecutionContext::ExecutionContext(const Modifier::Type modifiers) :
		ExecutionContext(modifiers, make_shared<symbol_map>(),
				ExecutionContextList::GetTerminator(), make_shared<TypeTable>(),
				PERSISTENT, 0) {
}

ExecutionContext::ExecutionContext(const Modifier::Type modifiers,
		const LifeTime lifetime) :
		ExecutionContext(modifiers, make_shared<symbol_map>(),
				ExecutionContextList::GetTerminator(), make_shared<TypeTable>(),
				lifetime, 0) {
}

ExecutionContext::ExecutionContext(const shared_ptr<SymbolContext> existing,
		volatile_shared_ptr<TypeTable> type_table, const LifeTime life_time) :
		ExecutionContext(existing, ExecutionContextList::GetTerminator(),
				type_table, life_time, 0) {
}

ExecutionContext::ExecutionContext(const shared_ptr<SymbolContext> existing,
		const Modifier::Type modifiers,
		const ExecutionContextListRef parent_context,
		volatile_shared_ptr<TypeTable> type_table, const LifeTime life_time,
		size_t depth) :
		ExecutionContext(existing, parent_context, type_table, life_time, depth) {
}

ExecutionContext::ExecutionContext(const Modifier::Type modifiers,
		const ExecutionContextListRef parent_context,
		volatile_shared_ptr<TypeTable> type_table, const LifeTime life_time,
		size_t depth) :
		ExecutionContext(modifiers, make_shared<symbol_map>(), parent_context,
				type_table, life_time, depth) {
}

const_shared_ptr<Symbol> ExecutionContext::GetSymbol(const string& identifier,
		const SearchType search_type) const {
	auto result = SymbolContext::GetSymbol(identifier);

	if (result == Symbol::GetDefaultSymbol() && m_parent
			&& search_type == DEEP) {
		assert(m_parent->GetData());
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
			new ExecutionContext(contents, m_parent, m_type_table, m_life_time,
					m_depth));
}

const shared_ptr<ExecutionContext> ExecutionContext::WithParent(
		const ExecutionContextListRef parent_context, bool modify_depth) const {
	return shared_ptr<ExecutionContext>(
			new ExecutionContext(GetModifiers(), GetTable(), parent_context,
					m_type_table->WithParent(
							parent_context->GetData()->GetTypeTable()),
					m_life_time,
					modify_depth && parent_context ?
							parent_context->GetData()->GetDepth() + 1 :
							GetDepth()));
}

const shared_ptr<ExecutionContext> ExecutionContext::GetRuntimeInstance(
		const shared_ptr<ExecutionContext> source,
		const shared_ptr<ExecutionContext> parent) {
	ExecutionContextListRef new_parent;
	volatile_shared_ptr<TypeTable> new_type_table = source->GetTypeTable();
	if (parent) {
		auto parent_context = ExecutionContextList::From(parent,
				parent->GetParent());

		new_parent = ExecutionContextList::From(parent, parent->GetParent());
		new_type_table = new_type_table->WithParent(parent->GetTypeTable());
	}

	auto execution_context = make_shared<ExecutionContext>(source->Clone(),
			Modifier::MUTABLE, new_parent, new_type_table, EPHEMERAL,
			parent->GetDepth() + 1);

	return execution_context;
}

ExecutionContext::ExecutionContext(const shared_ptr<SymbolContext> context,
		const ExecutionContextListRef parent_context,
		volatile_shared_ptr<TypeTable> type_table, const LifeTime life_time,
		size_t depth) :
		SymbolTable(*context), m_parent(parent_context), m_type_table(
				type_table), m_life_time(life_time), m_depth(depth) {
	assert(m_type_table);
	if (m_parent && m_parent->GetData()) {
		assert(m_type_table != m_parent->GetData()->GetTypeTable());
	}
}

ExecutionContext::ExecutionContext(const Modifier::Type modifiers,
		const shared_ptr<symbol_map> symbol_map,
		const ExecutionContextListRef parent_context,
		volatile_shared_ptr<TypeTable> type_table, const LifeTime life_time,
		size_t depth) :
		SymbolTable(modifiers, symbol_map), m_parent(parent_context), m_type_table(
				type_table), m_life_time(life_time), m_depth(depth) {
	assert(m_type_table);
	if (m_parent && m_parent->GetData()) {
		assert(m_type_table != m_parent->GetData()->GetTypeTable());
	}
}

ExecutionContext::~ExecutionContext() {
}

const shared_ptr<ExecutionContext> ExecutionContext::GetDefault() {
	static const shared_ptr<ExecutionContext> instance = make_shared<
			ExecutionContext>(Modifier::NONE,
			ExecutionContextList::GetTerminator(), TypeTable::GetDefault(),
			PERSISTENT, 0);
	return instance;
}

void ExecutionContext::LinkToParent(const shared_ptr<ExecutionContext> parent) {
	assert(m_type_table);
	assert(parent->GetTypeTable());

	ExecutionContextListRef new_parent = ExecutionContextList::From(parent,
			parent->GetParent());

	auto new_typetable = m_type_table->WithParent(parent->GetTypeTable());
	m_parent = new_parent;
	m_type_table = new_typetable;
	m_depth = parent->GetDepth() + 1;
}

const shared_ptr<ExecutionContext> ExecutionContext::GetEmptyChild(
		const shared_ptr<ExecutionContext> parent,
		const Modifier::Type modifiers, const LifeTime life_time) {
	return GetEmptyChild(parent, modifiers, life_time,
			make_shared<TypeTable>(parent->GetTypeTable()),
			make_shared<symbol_map>());
}

const shared_ptr<ExecutionContext> ExecutionContext::GetEmptyChild(
		const shared_ptr<ExecutionContext> parent,
		const Modifier::Type modifiers, const LifeTime life_time,
		volatile_shared_ptr<TypeTable> type_table) {
	return GetEmptyChild(parent, modifiers, life_time, type_table,
			make_shared<symbol_map>());
}

const shared_ptr<ExecutionContext> ExecutionContext::GetEmptyChild(
		const shared_ptr<ExecutionContext> parent,
		const Modifier::Type modifiers, const LifeTime life_time,
		volatile_shared_ptr<TypeTable> type_table,
		const shared_ptr<symbol_map> map) {
	auto new_parent = ExecutionContextList::From(parent, parent->GetParent());
	return shared_ptr<ExecutionContext>(
			new ExecutionContext(modifiers, map, new_parent, type_table,
					life_time, parent->GetDepth() + 1));
}

void ExecutionContext::WeakenReferences() {
	if ((GetModifiers() & Modifier::MUTABLE) == Modifier::MUTABLE
			&& GetLifeTime() != ROOT) {
		auto symbol_table = GetTable();

		symbol_map::iterator it;
		for (it = symbol_table->begin(); it != symbol_table->end(); it++) {
			auto symbol = it->second;
			if (symbol->IsWeakenable()) {
				auto new_symbol = Symbol::WeakenReference(symbol);
				it->second = new_symbol;
			}
		}
	}
}
