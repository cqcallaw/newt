#ifndef STATEMENT_BLOCK_H
#define STATEMENT_BLOCK_H

#include "linked_list.h"
#include "statement.h"

class Statement;

class StatementBlock {
public:
	StatementBlock(const LinkedList<const Statement*>* statements);

	bool empty();

	void execute() const;

private:
	const LinkedList<const Statement*>* m_statements;
};

#endif // #ifndef STATEMENT_BLOCK_H
