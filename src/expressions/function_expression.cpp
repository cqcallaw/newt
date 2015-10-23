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

#include <function_expression.h>
#include <function.h>
#include <statement_block.h>
#include <declaration_list.h>
#include <execution_context.h>
#include <function_type_specifier.h>

FunctionExpression::FunctionExpression(const YYLTYPE position,
		const DeclarationList* parameter_list, const TypeSpecifier* return_type,
		const StatementBlock* body) :
		Expression(position), m_type(
				new FunctionTypeSpecifier(parameter_list, return_type)), m_body(
				body) {
}

FunctionExpression::~FunctionExpression() {
}

const TypeSpecifier* FunctionExpression::GetType(
		const ExecutionContext* execution_context) const {
	return m_type;
}

const Result* FunctionExpression::Evaluate(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;
	const Function* function = new Function(m_type);
	return new Result(function, errors);
}

const bool FunctionExpression::IsConstant() const {
	return true;
}

const LinkedList<const Error*>* FunctionExpression::Validate(
		const ExecutionContext* execution_context) const {
	const LinkedList<const Error*>* errors =
			LinkedList<const Error*>::Terminator;

	//generate a temporary context for validation
	auto parent = execution_context->GetSymbolContext()->GetParent();
	auto new_parent = parent->With(execution_context->GetSymbolContext());
	SymbolContext* tmp_table = new SymbolContext(Modifier::Type::NONE,
			new_parent);
	const ExecutionContext* tmp_context = execution_context->WithSymbolContext(
			tmp_table);

	errors = m_body->preprocess(execution_context);

	AnalysisResult returns = m_body->Returns(m_type->GetReturnType(),
			execution_context);
	if (returns == AnalysisResult::NO) {
		errors = errors->With(
				new Error(Error::SEMANTIC, Error::FUNCTION_RETURN,
						GetPosition().first_line, GetPosition().first_column));
	}

	delete tmp_context;
	delete tmp_table;
	delete new_parent;

	return errors;
}
