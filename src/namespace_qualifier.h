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

#ifndef NAMESPACE_QUALIFIER_H_
#define NAMESPACE_QUALIFIER_H_

#include <defaults.h>
#include <linked_list.h>

class NamespaceQualifier {
public:
	NamespaceQualifier(const_shared_ptr<string> id) :
			m_id(id) {
	}

	virtual ~NamespaceQualifier() {
	}

	const_shared_ptr<string> GetId() const {
		return m_id;
	}

private:
	const_shared_ptr<string> m_id;
};

typedef const LinkedList<const NamespaceQualifier, NO_DUPLICATES> NamespaceQualifierList;
typedef shared_ptr<NamespaceQualifierList> NamespaceQualifierListRef;

#endif /* NAMESPACE_QUALIFIER_H_ */
