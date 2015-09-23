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

#ifndef STATEMENTS_STRUCT_INSTANTIATION_STATEMENT_H_
#define STATEMENTS_STRUCT_INSTANTIATION_STATEMENT_H_

#include <statement.h>
#include <string>
#include <yyltype.h>
#include <member_instantiation_list.h>
#include <type.h>

class Symbol;

using namespace std;

class StructInstantiationStatement: public Statement {
public:
	StructInstantiationStatement(const string* type_name,
			const YYLTYPE type_name_position, const string* name,
			const YYLTYPE name_position,
			const MemberInstantiationList* member_instantiation_list,
			const YYLTYPE member_instantiation_list_position);
	virtual ~StructInstantiationStatement();

	const MemberInstantiationList* GetMemberInstantiationList() const {
		return m_member_instantiation_list;
	}

	const YYLTYPE GetMemberInstantiationListPosition() const {
		return m_member_instantiation_list_position;
	}

	const string* GetName() const {
		return m_name;
	}

	const YYLTYPE GetNamePosition() const {
		return m_name_position;
	}

	const string* GetTypeName() const {
		return m_type_name;
	}

	const YYLTYPE GetTypeNamePosition() const {
		return m_type_name_position;
	}

	virtual const LinkedList<const Error*>* preprocess(
			const ExecutionContext* execution_context) const;

	virtual const LinkedList<const Error*>* execute(
			const ExecutionContext* execution_context) const;

private:
	const string* m_type_name;
	const YYLTYPE m_type_name_position;
	const string* m_name;
	const YYLTYPE m_name_position;
	const MemberInstantiationList* m_member_instantiation_list;
	const YYLTYPE m_member_instantiation_list_position;

	const Symbol* GetSymbol(const BasicType member_type,
			const string member_name, const void* void_value) const;
};

#endif /* STATEMENTS_STRUCT_INSTANTIATION_STATEMENT_H_ */
