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

const LinkedList<const Error*>* StructInstantiationStatement::preprocess(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;
	//TODO: validate that all members are initialized for readonly structs (?)

	const CompoundType* type = execution_context->GetTypeTable()->GetType(
			*m_type_name);

	if (type != CompoundType::GetDefaultCompoundType()) {
		SymbolTable* symbol_table =
				(SymbolTable*) execution_context->GetSymbolContext();

		const Symbol* existing = symbol_table->GetSymbol(m_name);
		if (existing == Symbol::DefaultSymbol) {
			const LinkedList<const MemberInstantiation*>* instatiation_list =
					m_member_instantiation_list;
			while (!instatiation_list->IsTerminator()) {
				const MemberInstantiation* instantiation =
						instatiation_list->GetData();
				const string* member_name = instantiation->GetName();
				//errors = instantiation->Validate(execution_context, type);
				const MemberDefinition* member_definition = type->GetMember(
						*member_name);
				if (member_definition
						!= MemberDefinition::GetDefaultMemberDefinition()) {
					const TypeSpecifier* member_type =
							member_definition->GetType();
					const TypeSpecifier* expression_type =
							instantiation->GetExpression()->GetType(
									execution_context);
					if (!expression_type->IsAssignableTo(member_type)) {
						//undefined member
						errors = errors->With(
								new Error(Error::SEMANTIC,
										Error::ASSIGNMENT_TYPE_ERROR,
										m_name_position.first_line,
										m_name_position.first_column,
										member_type->ToString(),
										expression_type->ToString()));
					}
				} else {
					//undefined member
					errors = errors->With(
							new Error(Error::SEMANTIC, Error::UNDECLARED_MEMBER,
									m_name_position.first_line,
									m_name_position.first_column, *member_name,
									member_definition->GetType()->ToString()));

				}

				instatiation_list = instatiation_list->GetNext();
			}

			if (errors->IsTerminator()) {
				//generate default instance
				const CompoundTypeInstance* instance =
						CompoundTypeInstance::GetDefaultInstance(*m_type_name,
								type);

				const Symbol* symbol = new Symbol(*m_name, instance);
				const InsertResult insert_result = symbol_table->InsertSymbol(
						symbol);

				if (insert_result != INSERT_SUCCESS) {
					assert(false);
				}
			}
		} else {
			//symbol already exists
			errors = errors->With(
					new Error(Error::SEMANTIC,
							Error::PREVIOUSLY_DECLARED_VARIABLE,
							m_type_name_position.first_line,
							m_type_name_position.first_column, *m_name));
		}
	} else {
		//type does not exist
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::INVALID_TYPE,
						m_type_name_position.first_line,
						m_type_name_position.first_column, *m_type_name));
	}

	return errors;
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

			if (member_type->IsAssignableTo(
					PrimitiveTypeSpecifier::GetBoolean())) {
				struct_symbol_context->SetSymbol(member_name,
						(bool*) void_value);
			} else if (member_type->IsAssignableTo(
					PrimitiveTypeSpecifier::GetInt())) {
				struct_symbol_context->SetSymbol(member_name,
						(int*) void_value);
			} else if (member_type->IsAssignableTo(
					PrimitiveTypeSpecifier::GetDouble())) {
				struct_symbol_context->SetSymbol(member_name,
						(double*) void_value);
			} else if (member_type->IsAssignableTo(
					PrimitiveTypeSpecifier::GetString())) {
				struct_symbol_context->SetSymbol(member_name,
						(string*) void_value);
			} else if (dynamic_cast<const CompoundTypeSpecifier*>(member_type)
					!= nullptr) {
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
