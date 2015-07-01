/*
 * execution_context.h
 *
 *  Created on: Jun 20, 2015
 *      Author: caleb
 */

#ifndef EXECUTION_CONTEXT_H_
#define EXECUTION_CONTEXT_H_

class SymbolTable;

class ExecutionContext {
public:
	ExecutionContext(const SymbolTable* symbol_table);
	virtual ~ExecutionContext();

	const SymbolTable* GetSymbolTable() const {
		return m_symbol_table;
	}

private:
	const SymbolTable* m_symbol_table;
};

#endif /* EXECUTION_CONTEXT_H_ */
