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
#include <map>
#include <execution_context.h>
#include <type_table.h>
#include <result.h>
#include <member_instantiation.h>
#include <expression.h>
#include <struct.h>
#include <symbol.h>
#include <member_definition.h>

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
	//TODO: check the initializer type matches that of the initialized member

	//generate default instance
	SymbolTable* symbol_table =
			(SymbolTable*) execution_context->GetSymbolContext();
	const LinkedList<const SymbolContext*>* parent_context =
			symbol_table->GetParent();
	const LinkedList<const SymbolContext*>* new_parent_context =
			parent_context->With(symbol_table);

	const CompoundType* type = execution_context->GetTypeTable()->GetType(
			*m_type_name);

	map<const string, const Symbol*> symbol_mapping;

	const map<const string, const MemberDefinition*>* type_definition =
			type->GetDefinition();
	map<const string, const MemberDefinition*>::const_iterator iter;

	for (iter = type_definition->begin(); iter != type_definition->end();
			++iter) {
		const string member_name = iter->first;
		const MemberDefinition* member_type_information = iter->second;

		const Symbol* symbol = GetSymbol(member_type_information->GetType(),
				member_name, member_type_information->GetDefaultValue());
		symbol_mapping.insert(
				std::pair<const string, const Symbol*>(member_name, symbol));
	}

	Struct* instance = new Struct(*m_type_name,
			new SymbolContext(new_parent_context, &symbol_mapping));

	const Symbol* symbol = new Symbol(*m_name, instance);
	const InsertResult insert_result = symbol_table->InsertSymbol(symbol);

	return LinkedList<const Error*>::Terminator;
}

const Symbol* StructInstantiationStatement::GetSymbol(
		const BasicType member_type, const string member_name,
		const void* void_value) const {
	switch (member_type) {
	case BOOLEAN:
		return new Symbol(member_name, (bool*) void_value);
		break;
	case INT:
		return new Symbol(member_name, (int*) void_value);
		break;
	case DOUBLE:
		return new Symbol(member_name, (double*) void_value);
		break;
	case STRING:
		return new Symbol(member_name, (string*) void_value);
		break;
	case STRUCT:
		return new Symbol(member_name, (Struct*) void_value);
		break;
	default:
		assert(false);
		return nullptr;
	}
}

const LinkedList<const Error*>* StructInstantiationStatement::execute(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;
	const Symbol* base = execution_context->GetSymbolContext()->GetSymbol(
			m_name);
	const Struct* base_struct = (const Struct*) base->GetValue();
	SymbolContext* struct_symbol_context = base_struct->GetDefinition();

	const LinkedList<const MemberInstantiation*>* subject =
			m_member_instantiation_list;
	while (subject != LinkedList<const MemberInstantiation*>::Terminator) {
		const MemberInstantiation* memberInstantiation = subject->GetData();
		const string member_name = *memberInstantiation->GetName();
		const Symbol* member_symbol = struct_symbol_context->GetSymbol(
				member_name);
		const BasicType member_type = member_symbol->GetType();
		const Result* evaluation_result =
				memberInstantiation->GetExpression()->Evaluate(
						execution_context);

		if (evaluation_result->GetErrors()
				== LinkedList<const Error*>::Terminator) {
			const void* void_value = evaluation_result->GetData();

			switch (member_type) {
			case BOOLEAN:
				struct_symbol_context->SetSymbol(member_name,
						(bool*) void_value);
				break;
			case INT:
				struct_symbol_context->SetSymbol(member_name,
						(int*) void_value);
				break;
			case DOUBLE:
				struct_symbol_context->SetSymbol(member_name,
						(double*) void_value);
				break;
			case STRING:
				struct_symbol_context->SetSymbol(member_name,
						(string*) void_value);
				break;
			case STRUCT:
				struct_symbol_context->SetSymbol(member_name,
						(Struct*) void_value);
				break;
			default:
				assert(false);
				return nullptr;
			}
		} else {
			return evaluation_result->GetErrors();
		}

		subject = subject->GetNext();
	}

	return errors;
}
