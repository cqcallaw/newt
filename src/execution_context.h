/*
 * execution_context.h
 *
 *  Created on: Jun 20, 2015
 *      Author: caleb
 */

#ifndef EXECUTION_CONTEXT_H_
#define EXECUTION_CONTEXT_H_

class SymbolTable;
class TypeTable;

class ExecutionContext {
public:
	ExecutionContext();
	ExecutionContext(SymbolTable* symbol_table, TypeTable* type_table);
	virtual ~ExecutionContext();

	SymbolTable* GetSymbolTable() const {
		return m_symbol_table;
	}

	TypeTable* GetTypeTable() const {
		return m_type_table;
	}

private:
	SymbolTable* m_symbol_table;
	TypeTable* m_type_table;
};

#endif /* EXECUTION_CONTEXT_H_ */
