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

class SymbolContext;
class TypeTable;

class ExecutionContext {
public:
	ExecutionContext();
	ExecutionContext(SymbolContext* symbol_context, TypeTable* type_table,
			bool dispose_members = false);
	virtual ~ExecutionContext();

	SymbolContext* GetSymbolContext() const {
		return m_symbol_context;
	}

	TypeTable* GetTypeTable() const {
		return m_type_table;
	}

	ExecutionContext* WithSymbolContext(SymbolContext* symbol_context) const {
		return new ExecutionContext(symbol_context, m_type_table);
	}

	const void* GetReturnValue() const {
		return m_return_value;
	}

	void SetReturnValue(const void* return_value) {
		m_return_value = return_value;
	}

	const static ExecutionContext* GetDefault();

private:
	ExecutionContext(SymbolContext* symbol_context, TypeTable* type_table,
			const void* m_return_value, bool dispose_members);

	SymbolContext* m_symbol_context;
	TypeTable* m_type_table;
	const void* m_return_value;
	const bool m_dispose_members;
};

#endif /* EXECUTION_CONTEXT_H_ */
