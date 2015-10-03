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
		const Expression* initialization_expression) :
		m_type_name(type_name), m_type_name_position(type_name_position), m_name(
				name), m_name_position(name_position), m_initialization_expression(
				initialization_expression) {
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
			const CompoundTypeInstance* instance = nullptr;
			if (m_initialization_expression) {
				const TypeSpecifier* expression_type =
						m_initialization_expression->GetType(execution_context);
				errors = m_initialization_expression->Validate(
						execution_context);

				if (errors->IsTerminator()) {
					const CompoundTypeSpecifier* as_compound_specifier =
							dynamic_cast<const CompoundTypeSpecifier*>(expression_type);

					if (as_compound_specifier
							&& m_type_name->compare(
									as_compound_specifier->GetTypeName())
									== 0) {
						//generate default instance
						instance = CompoundTypeInstance::GetDefaultInstance(
								*m_type_name, type);
					} else {
						errors =
								errors->With(
										new Error(Error::SEMANTIC,
												Error::ASSIGNMENT_TYPE_ERROR,
												m_initialization_expression->GetPosition().first_line,
												m_initialization_expression->GetPosition().first_column,
												*m_type_name,
												expression_type->ToString()));
					}
				}
			} else {
				instance = CompoundTypeInstance::GetDefaultInstance(
						*m_type_name, type);
			}

			if (errors->IsTerminator()) {
				//we've been able to get a good initial value (that is, no errors have occurred)
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
				new Error(Error::SEMANTIC, Error::UNDECLARED_TYPE,
						m_type_name_position.first_line,
						m_type_name_position.first_column, *m_type_name));
	}

	return errors;
}

const LinkedList<const Error*>* StructInstantiationStatement::execute(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	if (m_initialization_expression) {
		const Result* evaluation = m_initialization_expression->Evaluate(
				execution_context);

		errors = evaluation->GetErrors();

		if (errors->IsTerminator()) {
			const void* void_value = evaluation->GetData();
			const CompoundTypeInstance* instance =
					(const CompoundTypeInstance*) void_value;
			execution_context->GetSymbolContext()->SetSymbol(*m_name, instance);
		}
	}

	return errors;
}
