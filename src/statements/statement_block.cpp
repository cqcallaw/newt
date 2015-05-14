#include "statement_block.h"

#include <assert.h>
#include "statement.h"
#include "statement_list.h"

using namespace std;

StatementBlock::StatementBlock(const LinkedList<const Statement*>* statements) :
		m_statements(statements) {
}

bool StatementBlock::empty() {
	return m_statements == StatementList::Terminator;
}

void StatementBlock::execute() const {
	LinkedList<const Statement*>* list =
			(LinkedList<const Statement*>*) m_statements;
	while (list != StatementList::Terminator) {
		const Statement* statement = list->GetData();
		statement->execute();
		list = (LinkedList<const Statement*>*) list->GetNext();
	}
}
