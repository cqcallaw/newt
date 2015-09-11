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

#include <compound_type_instance.h>
#include <struct_instantiation_statement.h>
#include <map>
#include <execution_context.h>
#include <type_table.h>
#include <result.h>
#include <member_instantiation.h>
#include <expression.h>
#include <symbol.h>
#include <member_definition.h>
#include <compound_type.h>

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

	const CompoundTypeInstance* instance =
			CompoundTypeInstance::GetDefaultInstance(*m_type_name,
					new_parent_context, type);

	const Symbol* symbol = new Symbol(*m_name, instance);
	const InsertResult insert_result = symbol_table->InsertSymbol(symbol);

	return LinkedList<const Error*>::Terminator;
}

const LinkedList<const Error*>* StructInstantiationStatement::execute(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;
	const Symbol* base = execution_context->GetSymbolContext()->GetSymbol(
			m_name);
	const CompoundTypeInstance* base_struct =
			(const CompoundTypeInstance*) base->GetValue();
	SymbolContext* struct_symbol_context = base_struct->GetDefinition();

	const LinkedList<const MemberInstantiation*>* subject =
			m_member_instantiation_list;
	while (subject != LinkedList<const MemberInstantiation*>::Terminator) {
		const MemberInstantiation* memberInstantiation = subject->GetData();
		const string member_name = *memberInstantiation->GetName();
		const Symbol* member_symbol = struct_symbol_context->GetSymbol(
				member_name);
		const TypeSpecifier* member_type = member_symbol->GetType();
		const Result* evaluation_result =
				memberInstantiation->GetExpression()->Evaluate(
						execution_context);

		if (evaluation_result->GetErrors()
				== LinkedList<const Error*>::Terminator) {
			const void* void_value = evaluation_result->GetData();

			//switch (member_type) {
			if (member_type->IsAssignableTo(PrimitiveTypeSpecifier::BOOLEAN)) {
				struct_symbol_context->SetSymbol(member_name,
						(bool*) void_value);
			} else if (member_type->IsAssignableTo(
					PrimitiveTypeSpecifier::INT)) {
				struct_symbol_context->SetSymbol(member_name,
						(int*) void_value);
			} else if (member_type->IsAssignableTo(
					PrimitiveTypeSpecifier::DOUBLE)) {
				struct_symbol_context->SetSymbol(member_name,
						(double*) void_value);
			} else if (member_type->IsAssignableTo(
					PrimitiveTypeSpecifier::STRING)) {
				struct_symbol_context->SetSymbol(member_name,
						(string*) void_value);
			} else if (member_type->IsAssignableTo(
					PrimitiveTypeSpecifier::COMPOUND)) {
				struct_symbol_context->SetSymbol(member_name,
						(CompoundTypeInstance*) void_value);
			} else {
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
