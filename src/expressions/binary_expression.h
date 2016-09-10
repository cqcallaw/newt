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

#ifndef BINARY_EXPRESSION_H_
#define BINARY_EXPRESSION_H_

#include "expression.h"

class Error;

class BinaryExpression: public Expression {
public:
	BinaryExpression(const yy::location position, const OperatorType op,
			const_shared_ptr<Expression> left,
			const_shared_ptr<Expression> right);

	virtual TResult<TypeSpecifier> GetTypeSpecifier(
			const shared_ptr<ExecutionContext> execution_context,
			AliasResolution resolution = AliasResolution::RESOLVE) const;

	const_shared_ptr<Result> Evaluate(
			const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContext> closure) const;

	const_shared_ptr<Expression> GetLeft() const {
		return m_left;
	}
	const OperatorType GetOperator() const {
		return m_operator;
	}
	const_shared_ptr<Expression> GetRight() const {
		return m_right;
	}

	static TResult<TypeSpecifier> ComputeResultType(
			const_shared_ptr<Expression> left,
			const_shared_ptr<Expression> right, const OperatorType op,
			const shared_ptr<ExecutionContext> execution_context);

	virtual const bool IsConstant() const {
		return m_left->IsConstant() && m_right->IsConstant();
	}

	virtual const ErrorListRef Validate(
			const shared_ptr<ExecutionContext> execution_context,
			const_shared_ptr<TypeSpecifier> valid_left,
			const_shared_ptr<TypeSpecifier> valid_right) const;

protected:
	virtual const_shared_ptr<Result> compute(const bool& left,
			const bool& right, yy::location left_position,
			yy::location right_position) const = 0;
	const_shared_ptr<Result> compute(const bool& left, const int& right,
			yy::location left_position, yy::location right_position) const;
	const_shared_ptr<Result> compute(const bool& left, const double& right,
			yy::location left_position, yy::location right_position) const;
	const_shared_ptr<Result> compute(const bool& left, const string& right,
			yy::location left_position, yy::location right_position) const;

	const_shared_ptr<Result> compute(const int& left, const bool& right,
			yy::location left_position, yy::location right_position) const;
	virtual const_shared_ptr<Result> compute(const int& left, const int& right,
			yy::location left_position, yy::location right_position) const = 0;
	const_shared_ptr<Result> compute(const int& left, const double& right,
			yy::location left_position, yy::location right_position) const;
	const_shared_ptr<Result> compute(const int& left, const string& right,
			yy::location left_position, yy::location right_position) const;

	const_shared_ptr<Result> compute(const double& left, const bool& right,
			yy::location left_position, yy::location right_position) const;
	const_shared_ptr<Result> compute(const double& left, const int& right,
			yy::location left_position, yy::location right_position) const;
	virtual const_shared_ptr<Result> compute(const double& left,
			const double& right, yy::location left_position,
			yy::location right_position) const = 0;
	const_shared_ptr<Result> compute(const double& left, const string& right,
			yy::location left_position, yy::location right_position) const;

	const_shared_ptr<Result> compute(const string& left, const bool& right,
			yy::location left_position, yy::location right_position) const;
	const_shared_ptr<Result> compute(const string& left, const int& right,
			yy::location left_position, yy::location right_position) const;
	const_shared_ptr<Result> compute(const string& left, const double& right,
			yy::location left_position, yy::location right_position) const;
	virtual const_shared_ptr<Result> compute(const string& left,
			const string& right, yy::location left_position,
			yy::location right_position) const = 0;

private:
	const OperatorType m_operator;
	const_shared_ptr<Expression> m_left;
	const_shared_ptr<Expression> m_right;
}
;

#endif /* BINARY_EXPRESSION_H_ */
