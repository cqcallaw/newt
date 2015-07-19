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

LinkedList<const Error*>* StatementBlock::preprocess(
		const ExecutionContext* execution_context) const {
	LinkedList<const Error*>* errors = LinkedList<const Error*>::Terminator;
	LinkedList<const Statement*>* subject =
			(LinkedList<const Statement*>*) m_statements;
	while (subject != LinkedList<const Statement*>::Terminator) {
		const Statement* statement = subject->GetData();
		//TODO: handle nested statement blocks
		LinkedList<const Error*>* statement_errors = statement->preprocess(
				execution_context);
		errors = (LinkedList<const Error*>*) errors->Concatenate(
				statement_errors, true);
	}

	return errors;
}

void StatementBlock::execute(const ExecutionContext* execution_context) const {
	LinkedList<const Statement*>* list =
			(LinkedList<const Statement*>*) m_statements;
	while (list != StatementList::Terminator) {
		const Statement* statement = list->GetData();
		statement->execute(execution_context);
		list = (LinkedList<const Statement*>*) list->GetNext();
	}
}
