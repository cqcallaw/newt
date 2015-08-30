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

#include <struct_instantiation_statement.h>
#include <type.h>
#include <map>
#include <execution_context.h>
#include <type_table.h>
#include <result.h>
#include <member_instantiation.h>
#include <expression.h>
#include <struct.h>

StructInstantiationStatement::StructInstantiationStatement(
		const std::string* type_name, const YYLTYPE type_name_position,
		const std::string* name, const YYLTYPE name_position,
		const MemberInstantiationList* member_instantiation_list,
		const YYLTYPE member_instantiation_list_position) :
		m_type_name(type_name), m_type_name_position(type_name_position), m_name(
				name), m_name_position(name_position), m_member_instantiation_list(
				member_instantiation_list), m_member_instantiation_list_position(
				member_instantiation_list_position) {
}

StructInstantiationStatement::~StructInstantiationStatement() {
}

LinkedList<const Error*>* StructInstantiationStatement::preprocess(
		const ExecutionContext* execution_context) const {
	//TODO: verify type exists
	//TODO: validate that all members are initialized for readonly structs
	//TODO: verify referenced members are valid
	//TODO: check for existing instance with same name
	//TODO: check that member types the type of their initializers
	return LinkedList<const Error*>::Terminator;
}

const LinkedList<const Error*>* StructInstantiationStatement::execute(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;
	const CompoundType* type = execution_context->GetTypeTable()->GetType(
			*m_type_name);

	Struct* instance = new Struct();

	const LinkedList<const MemberInstantiation*>* subject =
			m_member_instantiation_list;
	while (subject != LinkedList<const MemberInstantiation*>::Terminator) {
		const MemberInstantiation* memberInstantiation = subject->GetData();
		const string memberName = *memberInstantiation->GetName();
		const MemberDefinition* member_type_information = type->at(memberName);
		const Result* evaluation_result =
				memberInstantiation->GetExpression()->Evaluate(
						execution_context);

		if (evaluation_result->GetErrors()
				== LinkedList<const Error*>::Terminator) {
			instance->insert(
					pair<const string, const MemberDefinition*>(memberName,
							new MemberDefinition(
									member_type_information->GetType(),
									evaluation_result->GetData())));

		} else {
			return evaluation_result->GetErrors();
		}

		subject = subject->GetNext();
	}

	CompoundType::const_iterator iter;
	for (iter = type->begin(); iter != type->end(); ++iter) {
		const string member_name = iter->first;
		const MemberDefinition* member_type_information = iter->second;

		map<const string, const void*> map_iter;
		if (instance->find(member_name) == instance->end()) {
			instance->insert(
					pair<const string, const MemberDefinition*>(member_name,
							member_type_information));
		}
	}

	SymbolTable* symbol_table = execution_context->GetSymbolTable();

	const Symbol* symbol = new Symbol(*m_name, instance);

	const InsertResult insert_result = symbol_table->InsertSymbol(symbol);

	return errors;
}
