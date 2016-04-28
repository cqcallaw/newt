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

#ifndef MODIFIER_H_
#define MODIFIER_H_

#include <defaults.h>

class Modifier {
public:
	enum Type {
		NONE = 0, READONLY = 1
	};

	Modifier(const Type type, const yy::location type_position) :
			m_type(type), m_type_position(type_position) {

	}

	const Type GetType() const {
		return m_type;
	}

	const yy::location GetTypePosition() const {
		return m_type_position;
	}

private:
	const Type m_type;
	const yy::location m_type_position;
};

typedef const LinkedList<const Modifier, NO_DUPLICATES> ModifierList;
typedef shared_ptr<ModifierList> ModifierListRef;

#endif /* MODIFIER_H_ */
