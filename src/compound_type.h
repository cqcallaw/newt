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

#ifndef COMPOUND_TYPE_H_
#define COMPOUND_TYPE_H_

#include <map>
#include <string>

class MemberDefinition;

using namespace std;
class CompoundType {
public:
	CompoundType(const map<const string, const MemberDefinition*>* definition);
	virtual ~CompoundType();
	const MemberDefinition* GetMember(const string name) const;

	const string ToString() const;

	const map<const string, const MemberDefinition*>* GetDefinition() const {
		return m_definition;
	}

	const static CompoundType* DefaultCompoundType;

private:
	const map<const string, const MemberDefinition*>* m_definition;
};

std::ostream &operator<<(std::ostream &os, const CompoundType &symbol);
#endif /* COMPOUND_TYPE_H_ */
