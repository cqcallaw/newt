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

#include <sum_type.h>
#include <result.h>
#include <primitive_declaration_statement.h>
#include <default_value_expression.h>
#include <variable_expression.h>
#include <with_expression.h>
#include <return_statement.h>
#include <basic_variable.h>
#include <statement_block.h>
#include <member_instantiation.h>
#include <function_declaration.h>
#include <nested_type_specifier.h>
#include <execution_context.h>
#include <primitive_type.h>
#include <record_declaration_statement.h>

const_shared_ptr<Sum> SumType::GetInstance(const string& variant,
		const ArgumentListRef arguments) {
}

const std::string SumType::ToString(const TypeTable& type_table,
		const Indent& indent) const {
	ostringstream os;
	Indent child_indent = indent + 1;
	m_type_table->print(os, child_indent);
	return os.str();
}

const_shared_ptr<TypeSpecifier> SumType::GetMemberType(
		const string& member_name) const {
	auto result = m_constructor_map->find(member_name);

	if (result != m_constructor_map->end()) {
		auto function = result->second;
		return function->GetType()->GetReturnType();
	} else {
		return PrimitiveTypeSpecifier::GetNone();
	}
}

const_shared_ptr<Result> SumType::Build(
		const_shared_ptr<ExecutionContext> execution_context,
		const_shared_ptr<SumTypeSpecifier> type_specifier,
		const DeclarationListRef member_declarations) {
	ErrorListRef errors = ErrorList::GetTerminator();

	DeclarationListRef subject = member_declarations;
	while (!DeclarationList::IsTerminator(subject)) {
		const_shared_ptr<DeclarationStatement> declaration = subject->GetData();

		const_shared_ptr<Expression> initializer_expression =
				declaration->GetInitializerExpression();
		if (initializer_expression && !initializer_expression->IsConstant()) {
			//if we have a non-constant initializer expression, generate an error
			yy::location position = initializer_expression->GetPosition();
			errors = ErrorList::From(
					make_shared<Error>(Error::SEMANTIC,
							Error::MEMBER_DEFAULTS_MUST_BE_CONSTANT,
							position.begin.line, position.begin.column),
					errors);
		}

		subject = subject->GetNext();
	}

	const shared_ptr<constructor_map> variants = make_shared<constructor_map>();
	const shared_ptr<TypeTable> types = make_shared<TypeTable>();
	if (ErrorList::IsTerminator(errors)) {
		//generate a record type containing type constructor functions
		DeclarationListRef subject = member_declarations;
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
						parameter, type_specifier);

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
						function_signature, statement_block, nullptr);

				variants->insert(
						pair<const string, const_shared_ptr<Function>>(
								*as_primitive->GetName(), function));

				auto basic_type =
						static_pointer_cast<const PrimitiveTypeSpecifier>(
								as_primitive->GetType())->GetBasicType();
				types->AddType(*as_primitive->GetName(),
						make_shared<PrimitiveType>(basic_type));
			}

			//for record types, take keyword arguments.
			//therefore, declaration statements map directly to named parameters
			auto as_record = dynamic_pointer_cast<
					const RecordDeclarationStatement>(declaration);
			if (as_record) {
				auto function_signature = make_shared<FunctionDeclaration>(
						as_record->GetMemberDeclarationList(),
						type_specifier);

				//source expression is (@sum_type).variant_name
				auto variant_specifier = make_shared<NestedTypeSpecifier>(
						type_specifier, variant_name);
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
						function_signature, statement_block, nullptr);

				auto record_type_name = as_record->GetName();
				variants->insert(
						pair<const string, const_shared_ptr<Function>>(
								*record_type_name, function));
				types->AddType(*record_type_name,
						execution_context->GetTypeTable()->GetType<
								TypeDefinition>(*record_type_name));
			}

			subject = subject->GetNext();
		}
	}

	auto type = const_shared_ptr<SumType>(
			new SumType(variants, types, member_declarations->GetData()));

	return make_shared<Result>(type, errors);
}
