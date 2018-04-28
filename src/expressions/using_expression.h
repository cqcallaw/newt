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

#ifndef EXPRESSIONS_USING_EXPRESSION_H_
#define EXPRESSIONS_USING_EXPRESSION_H_

#include <expression.h>

class StatementBlock;
class RecordType;

class UsingExpression: public Expression {
public:
	UsingExpression(const yy::location location,
			const_shared_ptr<Expression> expression,
			const_shared_ptr<string> identifier,
			const_shared_ptr<TypeSpecifier> return_type_specifier,
			const_shared_ptr<StatementBlock> body);
	virtual ~UsingExpression();

	virtual TypedResult<TypeSpecifier> GetTypeSpecifier(
			const shared_ptr<ExecutionContext> execution_context,
			AliasResolution resolution = AliasResolution::RESOLVE) const;

	virtual const_shared_ptr<Result> Evaluate(
			const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure) const;

	virtual const ErrorListRef Validate(
			const shared_ptr<ExecutionContext> execution_context,
			const_shared_ptr<type_specifier_map> type_specifier_mapping) const;

	virtual const bool IsConstant() const;

	static const_shared_ptr<std::string> TEARDOWN_NAME;
	static const_shared_ptr<std::string> VALUE_NAME;
	static const_shared_ptr<std::string> ERRORS_NAME;

private:
	const_shared_ptr<Expression> m_expression;
	const_shared_ptr<string> m_identifier;
	const_shared_ptr<TypeSpecifier> m_return_type_specifier;
	const_shared_ptr<StatementBlock> m_body;
	shared_ptr<ExecutionContext> m_block_context;

	static const ErrorListRef ValidateMember(
			const_shared_ptr<ComplexTypeSpecifier> expression_type_specifier,
			const_shared_ptr<TypeSpecifier> block_return_type_specifier,
			const yy::location expression_location,
			const_shared_ptr<TypeTable> type_table,
			const_shared_ptr<RecordType> source_type,
			const_shared_ptr<string> name);
};

#endif /* EXPRESSIONS_USING_EXPRESSION_H_ */
