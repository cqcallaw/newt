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

#ifndef EXECUTION_CONTEXT_H_
#define EXECUTION_CONTEXT_H_

#include <defaults.h>
#include <symbol_table.h>
#include <search_type.h>
#include <modifier.h>

class TypeTable;

enum LifeTime {
	PERSISTENT, TEMPORARY, EPHEMERAL, ROOT
};

#include <execution_context_list.h>
typedef shared_ptr<ExecutionContextList> ExecutionContextListRef;

class ExecutionContext: public SymbolTable {
	friend class MatchStatement;
public:
	using SymbolContext::GetSymbol;
	using SymbolContext::SetSymbol;

	ExecutionContext();
	ExecutionContext(const Modifier::Type modifiers);
	ExecutionContext(const Modifier::Type modifiers, const LifeTime lifetime);
	ExecutionContext(const shared_ptr<SymbolContext> existing,
			volatile_shared_ptr<TypeTable> type_table,
			const LifeTime life_time);
	ExecutionContext(const Modifier::Type modifiers,
			const ExecutionContextListRef parent_context,
			volatile_shared_ptr<TypeTable> type_table, const LifeTime life_time,
			size_t depth);
	ExecutionContext(const shared_ptr<SymbolContext> existing,
			const Modifier::Type modifiers,
			const ExecutionContextListRef parent_context,
			volatile_shared_ptr<TypeTable> type_table, const LifeTime life_time,
			size_t depth);
	virtual ~ExecutionContext();

	static const shared_ptr<ExecutionContext> GetEmptyChild(
			const shared_ptr<ExecutionContext> parent,
			const Modifier::Type modifiers, const LifeTime life_time);

	static const shared_ptr<ExecutionContext> GetEmptyChild(
			const shared_ptr<ExecutionContext> parent,
			const Modifier::Type modifiers, const LifeTime life_time,
			volatile_shared_ptr<TypeTable> type_table);

	static const shared_ptr<ExecutionContext> GetEmptyChild(
			const shared_ptr<ExecutionContext> parent,
			const Modifier::Type modifiers, const LifeTime life_time,
			volatile_shared_ptr<TypeTable> type_table,
			const shared_ptr<symbol_map> map);

	static const shared_ptr<ExecutionContext> GetRuntimeInstance(
			const shared_ptr<ExecutionContext> source,
			const shared_ptr<ExecutionContext> parent);

	const shared_ptr<ExecutionContext> WithContents(
			const shared_ptr<SymbolContext> contents) const;

	virtual const shared_ptr<ExecutionContext> WithParent(
			const ExecutionContextListRef parent_context, bool modify_depth =
					true) const;

	const ExecutionContextListRef GetParent() const {
		return m_parent;
	}

	const size_t GetDepth() const {
		return m_depth;
	}

	volatile_shared_ptr<TypeTable> GetTypeTable() const {
		return m_type_table;
	}

	static const shared_ptr<ExecutionContext> GetDefault();

	const LifeTime GetLifeTime() const {
		return m_life_time;
	}

	const_shared_ptr<Symbol> GetSymbol(const_shared_ptr<std::string> identifier,
			const SearchType search_type) const;
	const_shared_ptr<Symbol> GetSymbol(const std::string& identifier,
			const SearchType search_type) const;

	const void print(ostream &os, const TypeTable& type_table,
			const Indent& indent, const SearchType search_type = SHALLOW) const;

	void LinkToParent(const shared_ptr<ExecutionContext> parent);

	void WeakenReferences();

protected:
	virtual SetResult SetSymbol(const std::string& identifier,
			const_shared_ptr<TypeSpecifier> type, const_shared_ptr<void> value,
			const TypeTable& type_table);

private:
	ExecutionContext(const Modifier::Type modifiers,
			const shared_ptr<symbol_map>,
			const ExecutionContextListRef parent_context,
			volatile_shared_ptr<TypeTable> type_table, const LifeTime life_time,
			size_t depth);

	ExecutionContext(const shared_ptr<SymbolContext> context,
			const ExecutionContextListRef parent_context,
			volatile_shared_ptr<TypeTable> type_table, const LifeTime life_time,
			size_t depth);

	ExecutionContextListRef m_parent;
	volatile_shared_ptr<TypeTable> m_type_table;
	const LifeTime m_life_time;
	size_t m_depth;
};

#endif /* EXECUTION_CONTEXT_H_ */
