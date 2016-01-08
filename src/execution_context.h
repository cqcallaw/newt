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

class SymbolContext;
class TypeTable;

enum LifeTime {
	PERSISTENT, EPHEMERAL
};

class ExecutionContext {
public:
	ExecutionContext();
	ExecutionContext(volatile_shared_ptr<SymbolContext> symbol_context,
			volatile_shared_ptr<TypeTable> type_table,
			const LifeTime life_time);
	virtual ~ExecutionContext();

	volatile_shared_ptr<SymbolContext> GetSymbolContext() const {
		return m_symbol_context;
	}

	volatile_shared_ptr<TypeTable> GetTypeTable() const {
		return m_type_table;
	}

	volatile_shared_ptr<ExecutionContext> WithSymbolContext(
			volatile_shared_ptr<SymbolContext> symbol_context) const {
		return std::make_shared<ExecutionContext>(symbol_context, m_type_table,
				m_life_time);
	}

	plain_shared_ptr<void> GetReturnValue() const {
		return m_return_value;
	}

	void SetReturnValue(plain_shared_ptr<void> return_value) {
		m_return_value = return_value;
	}

	plain_shared_ptr<int> GetExitCode() const {
		return m_exit_code;
	}

	void SetExitCode(plain_shared_ptr<int> exit_code) {
		m_exit_code = exit_code;
	}

	static const_shared_ptr<ExecutionContext> GetDefault();

	const LifeTime GetLifeTime() const {
		return m_life_time;
	}

private:
	ExecutionContext(volatile_shared_ptr<SymbolContext> symbol_context,
			volatile_shared_ptr<TypeTable> type_table,
			plain_shared_ptr<void> m_return_value,
			plain_shared_ptr<int> exit_code, const LifeTime life_time);

	volatile_shared_ptr<SymbolContext> m_symbol_context;
	volatile_shared_ptr<TypeTable> m_type_table;
	plain_shared_ptr<void> m_return_value;
	plain_shared_ptr<int> m_exit_code;
	const LifeTime m_life_time;
};

#endif /* EXECUTION_CONTEXT_H_ */
