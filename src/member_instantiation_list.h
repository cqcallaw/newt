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

#ifndef MEMBER_INSTANTIATION_LIST_H_
#define MEMBER_INSTANTIATION_LIST_H_

#include "linked_list.h"

class MemberInstantiation;

class MemberInstantiationList: public LinkedList<const MemberInstantiation*> {
public:
	MemberInstantiationList(const MemberInstantiation* data,
			const MemberInstantiationList* next) :
			LinkedList(data, next) {
	}

	MemberInstantiationList(const LinkedList<const MemberInstantiation*>* list) :
			LinkedList(list) {
	}

	const bool IsConstant() const;

	static constexpr MemberInstantiationList* Terminator =
			(MemberInstantiationList*) LinkedList<const MemberInstantiation*>::Terminator;
};

#endif /* MEMBER_INSTANTIATION_LIST_H_ */
