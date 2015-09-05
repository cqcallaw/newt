/*
 * execution_context.h
 *
 *  Created on: Jun 20, 2015
 *      Author: caleb
 */

#ifndef EXECUTION_CONTEXT_H_
#define EXECUTION_CONTEXT_H_

class SymbolContext;
class TypeTable;

class ExecutionContext {
public:
	ExecutionContext();
	ExecutionContext(SymbolContext* symbol_context, TypeTable* type_table);
	virtual ~ExecutionContext();

	SymbolContext* GetSymbolContext() const {
		return m_symbol_context;
	}

	TypeTable* GetTypeTable() const {
		return m_type_table;
	}

	const ExecutionContext* WithSymbolContext(
			SymbolContext* symbol_context) const {
		return new ExecutionContext(symbol_context, m_type_table);
	}

private:
	SymbolContext* m_symbol_context;
	TypeTable* m_type_table;
};

#endif /* EXECUTION_CONTEXT_H_ */
