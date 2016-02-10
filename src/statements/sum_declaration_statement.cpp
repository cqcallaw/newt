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

#include <symbol_table.h>
#include <expression.h>
#include <execution_context.h>
#include <sum.h>
#include <sum_declaration_statement.h>
#include <basic_variable.h>
#include <default_value_expression.h>
#include <sum_type.h>
#include <record_type.h>
#include <member_instantiation.h>
#include <nested_type_specifier.h>
#include <primitive_type.h>
#include <record_declaration_statement.h>
#include <function_declaration.h>
#include <statement_block.h>
#include <member_definition.h>
#include <nested_type_specifier.h>
#include <primitive_declaration_statement.h>
#include <variable_expression.h>
#include <return_statement.h>
#include <with_expression.h>

SumDeclarationStatement::SumDeclarationStatement(const yy::location position,
		const_shared_ptr<ComplexTypeSpecifier> type,
		const_shared_ptr<string> name, const yy::location name_location,
		const DeclarationListRef variant_list,
		const yy::location variant_list_location) :
		DeclarationStatement(position, name, name_location,
				make_shared<DefaultValueExpression>(
						DefaultValueExpression(GetDefaultLocation(), type,
								variant_list_location)),
				ModifierList::GetTerminator(), GetDefaultLocation()), m_variant_list(
				variant_list), m_variant_list_location(variant_list_location), m_type(
				make_shared<SumTypeSpecifier>(type)) {
}

SumDeclarationStatement::~SumDeclarationStatement() {
}

const ErrorListRef SumDeclarationStatement::preprocess(
		const shared_ptr<ExecutionContext> execution_context) const {
	ErrorListRef errors = ErrorList::GetTerminator();

	auto type_table = execution_context->GetTypeTable();
	auto result = SumType::Build(*type_table, m_variant_list);

	errors = result->GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto type = static_pointer_cast<const SumType>(result->GetData());
		execution_context->GetTypeTable()->AddType(*GetName(), type);
	}

	//create a new read-only record type and record that contains the names of the variants mapped to type constructors
	auto constructor_map = make_shared<definition_map>();
	auto constructor_instance = make_shared<SymbolTable>();

	if (ErrorList::IsTerminator(errors)) {
		//generate a record type containing type constructor functions
		DeclarationListRef subject = m_variant_list;
		while (!DeclarationList::IsTerminator(subject)) {
			//in all cases, no arguments == default value
			//for primitive declaration statements, a single-argument function that just returns that argument

			auto declaration = subject->GetData();
			auto variant_name = declaration->GetName();

			auto as_primitive = dynamic_pointer_cast<
					const PrimitiveDeclarationStatement>(declaration);
			if (as_primitive) {
				const_shared_ptr<PrimitiveDeclarationStatement> parameter_declaration =
						as_primitive;
				DeclarationListRef parameter = DeclarationList::From(
						parameter_declaration,
						DeclarationList::GetTerminator());

				auto function_signature = make_shared<FunctionDeclaration>(
						parameter, m_type);

				const_shared_ptr<Expression> return_expression = make_shared<
						VariableExpression>(GetDefaultLocation(),
						make_shared<BasicVariable>(as_primitive->GetName(),
								GetDefaultLocation()));

				const_shared_ptr<ReturnStatement> return_statement =
						make_shared<ReturnStatement>(return_expression);

				const StatementListRef statement_list = StatementList::From(
						return_statement, StatementList::GetTerminator());
				const_shared_ptr<StatementBlock> statement_block = make_shared<
						StatementBlock>(statement_list, GetDefaultLocation());

				const_shared_ptr<Function> function = make_shared<Function>(
						function_signature, statement_block, execution_context);

				auto definition = make_shared<MemberDefinition>(
						function_signature, function);
				constructor_map->insert(
						pair<const string, const_shared_ptr<MemberDefinition>>(
								*variant_name, definition));

				auto symbol = make_shared<Symbol>(function);
				auto insert_result = constructor_instance->InsertSymbol(
						*variant_name, symbol);

				if (insert_result == SYMBOL_EXISTS) {
					errors =
							ErrorList::From(
									make_shared<Error>(Error::SEMANTIC,
											Error::PREVIOUS_DECLARATION,
											GetInitializerExpression()->GetPosition().begin.line,
											GetInitializerExpression()->GetPosition().begin.column,
											*variant_name), errors);
				}
			}

			//for record types, take keyword arguments.
			//therefore, declaration statements map directly to named parameters
			auto as_record = dynamic_pointer_cast<
					const RecordDeclarationStatement>(declaration);
			if (as_record) {
				auto variant_specifier = make_shared<NestedTypeSpecifier>(
						m_type, variant_name);

				auto function_signature = make_shared<FunctionDeclaration>(
						as_record->GetMemberDeclarationList(), m_type);

				//source expression is (@sum_type).variant_name
				const_shared_ptr<Expression> default_value_expression =
						make_shared<DefaultValueExpression>(
								GetDefaultLocation(), variant_specifier,
								GetDefaultLocation());

				//instantiation of each member is assigning the argument value to that member.
				//So we want a function definition that looks something like:
				// (member_a:member_a_type, member_b:member_b_type) -> sum_type {
				//		return @sum_type.variant with {
				//			member_a = member_a,
				//			member_b = member_b,
				//		}
				//}
				//the member instantiations are self-referential because the right-hand side is evaluated in the containing context
				DeclarationListRef member_subject =
						as_record->GetMemberDeclarationList();
				MemberInstantiationListRef member_instantiation_list =
						MemberInstantiationList::GetTerminator();

				while (!DeclarationList::IsTerminator(member_subject)) {
					auto member_declaration = member_subject->GetData();

					const_shared_ptr<Expression> member_instantionation_expression =
							make_shared<VariableExpression>(
									GetDefaultLocation(),
									make_shared<BasicVariable>(
											member_declaration->GetName(),
											GetDefaultLocation()));
					const_shared_ptr<MemberInstantiation> member_instantiation =
							make_shared<MemberInstantiation>(
									member_declaration->GetName(),
									member_declaration->GetNamePosition(),
									member_instantionation_expression);
					member_instantiation_list = MemberInstantiationList::From(
							member_instantiation, member_instantiation_list);

					member_subject = member_subject->GetNext();
				}

				const_shared_ptr<Expression> instantiation_expression =
						make_shared<WithExpression>(GetDefaultLocation(),
								default_value_expression,
								member_instantiation_list,
								GetDefaultLocation());

				const_shared_ptr<ReturnStatement> return_statement =
						make_shared<ReturnStatement>(instantiation_expression);

				const StatementListRef statement_list = StatementList::From(
						return_statement, StatementList::GetTerminator());
				const_shared_ptr<StatementBlock> statement_block = make_shared<
						StatementBlock>(statement_list, GetDefaultLocation());

				const_shared_ptr<Function> function = make_shared<Function>(
						function_signature, statement_block, execution_context);

				auto definition = make_shared<MemberDefinition>(
						function_signature, function);
				constructor_map->insert(
						pair<const string, const_shared_ptr<MemberDefinition>>(
								*variant_name, definition));
				auto symbol = make_shared<Symbol>(function);
				auto insert_result = constructor_instance->InsertSymbol(
						*variant_name, symbol);

				if (insert_result == SYMBOL_EXISTS) {
					errors =
							ErrorList::From(
									make_shared<Error>(Error::SEMANTIC,
											Error::PREVIOUS_DECLARATION,
											GetInitializerExpression()->GetPosition().begin.line,
											GetInitializerExpression()->GetPosition().begin.column,
											*variant_name), errors);
				}
			}

			subject = subject->GetNext();
		}
	}

	if (ErrorList::IsTerminator(errors)) {
		string ctor_type_name = *GetName() + "_ctor_map";
		auto specifier = make_shared<RecordTypeSpecifier>(ctor_type_name);
		auto type = make_shared<RecordType>(constructor_map,
				Modifier::Type::READONLY);
		type_table->AddType(ctor_type_name, type);

		auto instance = make_shared<Record>(specifier, constructor_instance);
		auto instance_symbol = make_shared<Symbol>(instance);
		auto insert_result = execution_context->InsertSymbol(*GetName(),
				instance_symbol);
		if (insert_result == SYMBOL_EXISTS) {
			errors =
					ErrorList::From(
							make_shared<Error>(Error::SEMANTIC,
									Error::PREVIOUS_DECLARATION,
									GetInitializerExpression()->GetPosition().begin.line,
									GetInitializerExpression()->GetPosition().begin.column,
									*GetName()), errors);
		}
	}

	return errors;
}

const ErrorListRef SumDeclarationStatement::execute(
		shared_ptr<ExecutionContext> execution_context) const {
	return ErrorList::GetTerminator();
}

const DeclarationStatement* SumDeclarationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
//no-op
	return new SumDeclarationStatement(GetPosition(), m_type, GetName(),
			GetNamePosition(), m_variant_list, m_variant_list_location);
}

const_shared_ptr<TypeSpecifier> SumDeclarationStatement::GetType() const {
	return m_type;
}
