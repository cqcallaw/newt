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

#include <error.h>
#include <symbol_table.h>
#include <execution_context.h>
#include <expression.h>
#include <assignment_statement.h>
#include <basic_variable.h>
#include <primitive_declaration_statement.h>
#include <variable_expression.h>
#include <complex_type.h> // for the type mapping definitions

PrimitiveDeclarationStatement::PrimitiveDeclarationStatement(
		const yy::location position,
		const_shared_ptr<TypeSpecifier> type_specifier,
		const yy::location type_specifier_location,
		const_shared_ptr<string> name, const yy::location name_position,
		const_shared_ptr<Expression> initializer_expression) :
		DeclarationStatement(position, name, name_position,
				initializer_expression, ModifierList::GetTerminator(),
				GetDefaultLocation()), m_type_specifier(type_specifier), m_type_specifier_location(
				type_specifier_location) {
	assert(dynamic_pointer_cast<const PrimitiveTypeSpecifier>(type_specifier));
}

PrimitiveDeclarationStatement::~PrimitiveDeclarationStatement() {
}

const PreprocessResult PrimitiveDeclarationStatement::Preprocess(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure,
		const TypeSpecifierListRef type_parameter_list,
		const_shared_ptr<TypeSpecifier> return_type_specifier) const {
	auto errors = ErrorList::GetTerminator();
	auto symbol = Symbol::GetDefaultSymbol();

	auto type_table = context->GetTypeTable();
	auto type_specifier_mapping_result = ComplexType::GetTypeParameterMap(
			type_parameter_list, m_type_specifier->GetTypeArgumentList(),
			type_table);

	errors = type_specifier_mapping_result.GetErrors();
	if (ErrorList::IsTerminator(errors)) {
		auto type_specifier_mapping = type_specifier_mapping_result.GetData();

		//validate the contents of our initialization expression
		if (GetInitializerExpression()) {
			errors = GetInitializerExpression()->Validate(context,
					type_specifier_mapping);
		}

		assert(
				std::dynamic_pointer_cast<const PrimitiveTypeSpecifier>(
						m_type_specifier));

		if (m_type_specifier != PrimitiveTypeSpecifier::GetNone()) {
			//only process types that aren't NONE
			//this prevents extraneous and confusing error messages by invalid type-inferred variable declaration

			//validate assignment of initializer type to our variable type
			auto type_table = context->GetTypeTable();

			if (GetInitializerExpression()) {
				auto expression_type_specifier_result =
						GetInitializerExpression()->GetTypeSpecifier(context);

				auto expression_type_specifier_result_errors =
						expression_type_specifier_result.GetErrors();
				if (ErrorList::IsTerminator(
						expression_type_specifier_result_errors)) {
					auto expression_type_specifier =
							expression_type_specifier_result.GetData();
					auto expression_as_primitive = std::dynamic_pointer_cast<
							const PrimitiveTypeSpecifier>(
							expression_type_specifier);
					if (expression_as_primitive == nullptr
							|| !expression_as_primitive->AnalyzeAssignmentTo(
									m_type_specifier, type_table)) {
						//the type specifier isn't primitive
						//-or-
						//we cannot assign a value of the expression type to the variable type
						errors =
								ErrorList::From(
										make_shared<Error>(Error::SEMANTIC,
												Error::INVALID_INITIALIZER_TYPE,
												GetInitializerExpression()->GetLocation().begin,
												*GetName(),
												m_type_specifier->ToString(),
												expression_type_specifier->ToString()),
										errors);
					}
				}
				//we purposefully ignore type specifier result errors here, as the initializer expression was validated at the top of the function
			}

			auto type_result = m_type_specifier->GetType(type_table);

			auto type_errors = type_result->GetErrors();
			if (ErrorList::IsTerminator(type_errors)) {
				auto type = type_result->GetData<TypeDefinition>();
				auto value = type->GetDefaultValue(type_table,
						TypeSpecifier::DefaultTypeSpecifierMap);
				symbol = type->GetSymbol(type_table, m_type_specifier, value,
						TypeSpecifier::DefaultTypeSpecifierMap);
			} else {
				errors = ErrorList::Concatenate(errors, type_errors);
			}

			if (symbol != Symbol::GetDefaultSymbol()) {
				//we've successfully generated a symbol for insertion
				InsertResult insert_result = context->InsertSymbol(*GetName(),
						symbol);
				if (insert_result == SYMBOL_EXISTS) {
					errors = ErrorList::From(
							make_shared<Error>(Error::SEMANTIC,
									Error::PREVIOUS_DECLARATION,
									GetNameLocation().begin, *GetName()),
							errors);
				}
			}
		}
	}

	return PreprocessResult(PreprocessResult::ReturnCoverage::NONE, errors);
}

const ExecutionResult PrimitiveDeclarationStatement::Execute(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	if (GetInitializerExpression()) {
		Variable* temp_variable = new BasicVariable(GetName(),
				GetNameLocation());
		auto errors = temp_variable->AssignValue(context, closure,
				GetInitializerExpression(), AssignmentType::ASSIGN);
		delete (temp_variable);

		return ExecutionResult(errors);
	} else {
		return ExecutionResult();
	}
}

const DeclarationStatement* PrimitiveDeclarationStatement::WithInitializerExpression(
		const_shared_ptr<Expression> expression) const {
	return new PrimitiveDeclarationStatement(GetLocation(), m_type_specifier,
			m_type_specifier_location, GetName(), GetNameLocation(), expression);
}
