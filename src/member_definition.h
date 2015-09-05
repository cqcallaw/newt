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

#ifndef MEMBER_DEFINITION_H_
#define MEMBER_DEFINITION_H_

#include <type.h>

class MemberDefinition {
public:
	MemberDefinition(const BasicType type, const void* value) :
			m_type(type), m_value(value) {
	}
	virtual ~MemberDefinition() {
	}

	const BasicType GetType() const {
		return m_type;
	}
	const void* GetDefaultValue() const {
		return m_value;
	}
private:
	const BasicType m_type;
	const void* m_value;
};

#endif /* MEMBER_DEFINITION_H_ */
