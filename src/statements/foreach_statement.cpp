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

#include <foreach_statement.h>
#include <expression.h>
#include <statement_block.h>
#include <execution_context.h>

ForeachStatement::ForeachStatement(const_shared_ptr<string> name,
		const_shared_ptr<Expression> expression,
		const_shared_ptr<StatementBlock> statement_block) :
		m_name(name), m_expression(expression), m_statement_block(
				statement_block), m_block_context(
				make_shared<ExecutionContext>(Modifier::Type::MUTABLE)) {
}

ForeachStatement::~ForeachStatement() {
}

const PreprocessResult ForeachStatement::Preprocess(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure,
		const_shared_ptr<TypeSpecifier> return_type_specifier) const {
	auto type_table = context->GetTypeTable();
	auto errors = ErrorList::GetTerminator();
	m_block_context->LinkToParent(context);

	auto expression_type_specifier_result = m_expression->GetTypeSpecifier(
			context, RESOLVE);
	auto expression_type_specifier_errors =
			expression_type_specifier_result.GetErrors();

	auto return_coverage = PreprocessResult::ReturnCoverage::NONE;
	if (ErrorList::IsTerminator(expression_type_specifier_errors)) {
		auto validation_errors = m_expression->Validate(context);

		if (ErrorList::IsTerminator(validation_errors)) {
			auto expression_type_specifier =
					expression_type_specifier_result.GetData();

			auto expression_type_result = expression_type_specifier->GetType(
					type_table, RESOLVE);
			auto expression_type_errors = expression_type_result->GetErrors();
			if (ErrorList::IsTerminator(expression_type_errors)) {
				auto expression_type = expression_type_result->GetData<
						TypeDefinition>();
				const_shared_ptr<void> default_value =
						expression_type->GetDefaultValue(*type_table);
				const_shared_ptr<Symbol> default_symbol =
						expression_type->GetSymbol(context->GetTypeTable(),
								expression_type_specifier, default_value);
				m_block_context->InsertSymbol(*m_name, default_symbol);
			} else {
				errors = expression_type_errors;
			}
		} else {
			errors = validation_errors;
		}
	} else {
		errors = expression_type_specifier_errors;
	}

	auto block_preprocess_result = m_statement_block->Preprocess(
			m_block_context, return_type_specifier);
	return_coverage = block_preprocess_result.GetReturnCoverage();

	errors = ErrorList::Concatenate(errors,
			block_preprocess_result.GetErrors());

	return PreprocessResult(return_coverage, errors);
}

const ErrorListRef ForeachStatement::Execute(
		const shared_ptr<ExecutionContext> context,
		const shared_ptr<ExecutionContext> closure) const {
	auto errors = ErrorList::GetTerminator();
	return errors;
}
