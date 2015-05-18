/*
 * statement_list.h
 *
 *  Created on: Mar 31, 2015
 *      Author: caleb
 */

#ifndef STATEMENT_LIST_H_
#define STATEMENT_LIST_H_

#include "linked_list.h"

class Statement;

class StatementList: public LinkedList<const Statement*> {
public:
	StatementList(const Statement* data, const StatementList* next) :
			LinkedList(data, next) {
	}

	StatementList(const LinkedList<const Statement*>* list) :
			LinkedList(list) {
	}

	static constexpr StatementList* Terminator = (StatementList*) LinkedList<
			const Statement*>::Terminator;
};

#endif /* STATEMENT_LIST_H_ */
