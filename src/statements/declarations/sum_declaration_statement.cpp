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
#include <basic_variable.h>
#include <primitive_declaration_statement.h>
#include <record_declaration_statement.h>
#include <sum_declaration_statement.h>
#include <type_alias_declaration_statement.h>
#include <default_value_expression.h>
#include <sum_type.h>
#include <record_type.h>
#include <member_instantiation.h>

#include <primitive_type.h>
#include <function_declaration.h>
#include <statement_block.h>
#include <record.h>
#include <variable_expression.h>
#include <return_statement.h>
#include <with_expression.h>
#include <function_type.h>
#include <placeholder_type.h>
#include <maybe_type.h>

SumDeclarationStatement::SumDeclarationStatement(const yy::location position,
		const_shared_ptr<ComplexTypeSpecifier> type_specifier,
		const TypeSpecifierListRef type_parameters,
		const yy::location type_parameter_location,
		const_shared_ptr<string> name, const yy::location name_location,
		const DeclarationListRef variant_list,
		const yy::location variant_list_location) :
		DeclarationStatement(position, name, name_location,
				make_shared<DefaultValueExpression>(GetDefaultLocation(),
						type_specifier, variant_list_location),
				ModifierList::GetTerminator(), GetDefaultLocation()), m_variant_list(
				variant_list), m_variant_list_location(variant_list_location), m_type_parameters(
				type_parameters), m_type_parameter_location(
				type_parameter_location), m_type_specifier(
				make_shared<SumTypeSpecifier>(type_specifier)) {
}

SumDeclarationStatement::~SumDeclarationStatement() {
}

const PreprocessResult SumDeclarationStatement::Preprocess(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure,
		const_shared_ptr<TypeSpecifier> return_type_specifier) const {
	ErrorListRef errors = ErrorList::GetTerminator();
	auto type_table = context->GetTypeTable();

	if (!type_table->ContainsType(*m_type_specifier)) {
		const_shared_ptr<Sum> default_value = make_shared<Sum>(
				make_shared<string>("placeholder tag"), make_shared<int>(0));
		auto placeholder_symbol = make_shared<Symbol>(m_type_specifier,
				default_value);
		auto placeholder_maybe_result = MaybeType::Build(closure,
				GetTypeSpecifier());

		errors = placeholder_maybe_result->GetErrors();
		if (ErrorList::IsTerminator(errors)) {
			auto placeholder_maybe =
					placeholder_maybe_result->GetData<MaybeType>();
			auto forward_declaration = make_shared<PlaceholderType>(GetName(),
					placeholder_symbol, placeholder_maybe);
			type_table->AddType(*GetName(), forward_declaration);
			auto result = SumType::Build(context, closure, m_variant_list,
					m_type_specifier);

			errors = result->GetErrors();
			if (ErrorList::IsTerminator(errors)) {
				auto type = result->GetData<SumType>();
				type_table->AddType(*GetName(), type);
			} else {
				type_table->RemoveTypeDefinition<PlaceholderType>(GetName());
			}
		}

//// this section of code is the seed of kwargs-based type constructors for record types
//// this feature does not seem compelling enough to implement at this time, but may have use later
////			//for record types, take keyword arguments.
////			//therefore, declaration statements map directly to named parameters
////			auto as_record = dynamic_pointer_cast<
////					const RecordDeclarationStatement>(declaration);
////			if (as_record) {
////				auto variant_specifier = make_shared<NestedTypeSpecifier>(
////						m_type, variant_name);
////
////				auto function_signature = make_shared<FunctionDeclaration>(
////						as_record->GetMemberDeclarationList(), m_type);
////
////				//source expression is (@sum_type).variant_name
////				const_shared_ptr<Expression> default_value_expression =
////						make_shared<DefaultValueExpression>(
////								GetDefaultLocation(), variant_specifier,
////								GetDefaultLocation());
////
////				//instantiation of each member is assigning the argument value to that member.
////				//So we want a function definition that looks something like:
////				// (member_a:member_a_type, member_b:member_b_type) -> sum_type {
////				//		return @sum_type.variant with {
////				//			member_a = member_a,
////				//			member_b = member_b,
////				//		}
////				//}
////				//the member instantiations are self-referential because the right-hand side is evaluated in the containing context
////				DeclarationListRef member_subject =
////						as_record->GetMemberDeclarationList();
////				MemberInstantiationListRef member_instantiation_list =
////						MemberInstantiationList::GetTerminator();
////
////				while (!DeclarationList::IsTerminator(member_subject)) {
////					auto member_declaration = member_subject->GetData();
////
////					const_shared_ptr<Expression> member_instantionation_expression =
////							make_shared<VariableExpression>(
////									GetDefaultLocation(),
////									make_shared<BasicVariable>(
////											member_declaration->GetName(),
////											GetDefaultLocation()));
////					const_shared_ptr<MemberInstantiation> member_instantiation =
////							make_shared<MemberInstantiation>(
////									member_declaration->GetName(),
////									member_declaration->GetNamePosition(),
////									member_instantionation_expression);
////					member_instantiation_list = MemberInstantiationList::From(
////							member_instantiation, member_instantiation_list);
////
////					member_subject = member_subject->GetNext();
////				}
////
////				const_shared_ptr<Expression> instantiation_expression =
////						make_shared<WithExpression>(GetDefaultLocation(),
////								default_value_expression,
////								member_instantiation_list,
////								GetDefaultLocation());
////
////				const_shared_ptr<ReturnStatement> return_statement =
////						make_shared<ReturnStatement>(instantiation_expression);
////
////				const StatementListRef statement_list = StatementList::From(
////						return_statement, StatementList::GetTerminator());
////				const_shared_ptr<StatementBlock> statement_block = make_shared<
////						StatementBlock>(statement_list, GetDefaultLocation());
////
////				const_shared_ptr<Function> function = make_shared<Function>(
////						function_signature, statement_block, execution_context);
////
////				auto definition = make_shared<MemberDefinition>(
////						function_signature, function);
////				constructor_map->insert(
////						pair<const string, const_shared_ptr<MemberDefinition>>(
////								*variant_name, definition));
////				auto symbol = make_shared<Symbol>(function);
////				auto insert_result = constructor_instance->InsertSymbol(
////						*variant_name, symbol);
////
////				if (insert_result == SYMBOL_EXISTS) {
////					errors =
////							ErrorList::From(
////									make_shared<Error>(Error::SEMANTIC,
////											Error::PREVIOUS_DECLARATION,
////											GetInitializerExpression()->GetLocation().begin,
////											*variant_name), errors);
////				}
////			}
//
//					subject = subject->GetNext();
//				}
//			}

	} else {
		errors = ErrorList::From(
				make_shared<Error>(Error::SEMANTIC, Error::PREVIOUS_DECLARATION,
						GetNameLocation().begin, *GetName()), errors);
	}

	return PreprocessResult(PreprocessResult::ReturnCoverage::NONE, errors);
}

const ExecutionResult SumDeclarationStatement::Execute(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	return ExecutionResult();
}

const DeclarationStatement* SumDeclarationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
	//no-op
	return new SumDeclarationStatement(GetLocation(), m_type_specifier,
			m_type_parameters, m_type_parameter_location, GetName(),
			GetNameLocation(), m_variant_list, m_variant_list_location);
}

const_shared_ptr<TypeSpecifier> SumDeclarationStatement::GetTypeSpecifier() const {
	return m_type_specifier;
}

const yy::location SumDeclarationStatement::GetTypeSpecifierLocation() const {
	return GetNameLocation();
}
