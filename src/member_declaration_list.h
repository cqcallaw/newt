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

#ifndef MEMBER_DECLARATION_LIST_H_
#define MEMBER_DECLARATION_LIST_H_

#include "linked_list.h"

class MemberDeclaration;

class MemberDeclarationList: public LinkedList<const MemberDeclaration*> {
public:
	MemberDeclarationList(const MemberDeclaration* data,
			const MemberDeclarationList* next) :
			LinkedList(data, next) {
	}

	MemberDeclarationList(const LinkedList<const MemberDeclaration*>* list) :
			LinkedList(list) {
	}

	static constexpr MemberDeclarationList* Terminator =
			(MemberDeclarationList*) LinkedList<const MemberDeclaration*>::Terminator;
};

#endif /* MEMBER_DECLARATION_LIST_H_ */
