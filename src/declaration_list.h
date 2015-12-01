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

#ifndef DECLARATION_LIST_H_
#define DECLARATION_LIST_H_

#include "linked_list.h"
#include "type_list.h"
#include "declaration_statement.h"

class TypeSpecifier;

class DeclarationList: public LinkedList<const DeclarationStatement*> {
public:
	DeclarationList(const DeclarationStatement* data,
			const DeclarationList* next) :
			LinkedList(data, next) {
	}

	DeclarationList(const LinkedList<const DeclarationStatement*>* list) :
			LinkedList(list) {
	}

	const TypeList* GetTypeList() const {
		const LinkedList<const DeclarationStatement*>* subject = this;
		const LinkedList<const TypeSpecifier*>* result = TypeList::Terminator;
		while (subject != Terminator) {
			const DeclarationStatement* statement = subject->GetData();
			const TypeSpecifier* type = statement->GetType();
			result = result->With(type);
			subject = subject->GetNext();
		}

		return result->IsTerminator() ?
				TypeList::Terminator : new TypeList(result->Reverse(true));
	}

	static constexpr DeclarationList* Terminator =
			(DeclarationList*) LinkedList<const DeclarationStatement*>::Terminator;
};

#endif /* DECLARATION_LIST_H_ */
