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
#include <modifier.h>

class TypeTable;

enum LifeTime {
	PERSISTENT, EPHEMERAL
};

enum SearchType {
	SHALLOW = 0, DEEP = 1
};

#include <symbol_context_list.h>
typedef shared_ptr<SymbolContextList> SymbolContextListRef;

class ExecutionContext: public SymbolTable {
public:
	using SymbolContext::GetSymbol;
	using SymbolContext::SetSymbol;

	ExecutionContext();
	ExecutionContext(const shared_ptr<SymbolContext> existing,
			volatile_shared_ptr<TypeTable> type_table,
			const LifeTime life_time);
	ExecutionContext(const Modifier::Type modifiers,
			const SymbolContextListRef parent_context,
			volatile_shared_ptr<TypeTable> type_table,
			const LifeTime life_time);
	virtual ~ExecutionContext();

	const shared_ptr<ExecutionContext> WithContents(
			const shared_ptr<SymbolContext> contents) const {
		return shared_ptr<ExecutionContext>(
				new ExecutionContext(contents, m_parent, m_type_table,
						m_return_value, m_exit_code, m_life_time));
	}

	const SymbolContextListRef GetParent() const {
		return m_parent;
	}

	virtual const shared_ptr<ExecutionContext> WithParent(
			const SymbolContextListRef parent_context) const {
		return shared_ptr<ExecutionContext>(
				new ExecutionContext(GetModifiers(), GetTable(), parent_context,
						m_type_table, m_return_value, m_exit_code, m_life_time));
	}

	volatile_shared_ptr<TypeTable> GetTypeTable() const {
		return m_type_table;
	}

	const_shared_ptr<void> GetReturnValue() const {
		return m_return_value;
	}

	void SetReturnValue(const_shared_ptr<void> return_value) {
		m_return_value = return_value;
	}

	const_shared_ptr<int> GetExitCode() const {
		return m_exit_code;
	}

	void SetExitCode(plain_shared_ptr<int> exit_code) {
		m_exit_code = exit_code;
	}

	static const shared_ptr<ExecutionContext> GetDefault();

	const LifeTime GetLifeTime() const {
		return m_life_time;
	}

	const_shared_ptr<Symbol> GetSymbol(const_shared_ptr<string> identifier,
			const SearchType search_type) const;
	const_shared_ptr<Symbol> GetSymbol(const string& identifier,
			const SearchType search_type) const;

	const void print(ostream &os, const TypeTable& type_table,
			const Indent& indent, const SearchType search_type = SHALLOW) const;

protected:
	virtual SetResult SetSymbol(const string& identifier,
			const_shared_ptr<TypeSpecifier> type, const_shared_ptr<void> value);

private:
	ExecutionContext(const Modifier::Type modifiers,
			const shared_ptr<symbol_map>,
			const SymbolContextListRef parent_context,
			volatile_shared_ptr<TypeTable> type_table,
			const_shared_ptr<void> return_value,
			const_shared_ptr<int> exit_code, const LifeTime life_time);

	ExecutionContext(const shared_ptr<SymbolContext> context,
			const SymbolContextListRef parent_context,
			volatile_shared_ptr<TypeTable> type_table,
			const_shared_ptr<void> return_value,
			const_shared_ptr<int> exit_code, const LifeTime life_time);

	const SymbolContextListRef m_parent;
	volatile_shared_ptr<TypeTable> m_type_table;
	plain_shared_ptr<void> m_return_value;
	plain_shared_ptr<int> m_exit_code;
	const LifeTime m_life_time;
};

#endif /* EXECUTION_CONTEXT_H_ */
