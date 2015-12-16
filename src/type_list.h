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

#ifndef TYPE_LIST_H_
#define TYPE_LIST_H_

#include "linked_list.h"

class TypeSpecifier;

class TypeList: public LinkedList<const TypeSpecifier*> {
public:
	TypeList(const TypeSpecifier* data, const TypeList* next) :
			LinkedList(data, next) {
	}

	TypeList(const LinkedList<const TypeSpecifier*>* list) :
			LinkedList(list) {
	}

	TypeList(const TypeSpecifier* data) :
			TypeList(data, TypeList::GetTerminator()) {
	}

	const static TypeList* GetTerminator() {
		static TypeList* terminator = (TypeList*) LinkedList<
				const TypeSpecifier*>::GetTerminator();
		return terminator;
	}
};

#endif /* TYPE_LIST_H_ */
