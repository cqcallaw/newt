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

#ifndef COMPARISON_EXPRESSION_H_
#define COMPARISON_EXPRESSION_H_

#include "binary_expression.h"

using namespace std;

class ComparisonExpression: public BinaryExpression {
public:
	ComparisonExpression(const yy::location position, const OperatorType op,
			const_shared_ptr<Expression> left,
			const_shared_ptr<Expression> right);

	virtual TypedResult<TypeSpecifier> GetTypeSpecifier(
			const shared_ptr<ExecutionContext> execution_context,
			AliasResolution resolution = AliasResolution::RESOLVE) const;

	virtual const ErrorListRef Validate(
			const shared_ptr<ExecutionContext> execution_context) const;

protected:
	virtual const_shared_ptr<Result> compute(const bool& left,
			const bool& right, yy::location left_position,
			yy::location right_position) const;
	virtual const_shared_ptr<Result> compute(const std::uint8_t& left,
			const std::uint8_t& right, yy::location left_position,
			yy::location right_position) const;
	virtual const_shared_ptr<Result> compute(const int& left, const int& right,
			yy::location left_position, yy::location right_position) const;
	virtual const_shared_ptr<Result> compute(const double& left,
			const double& right, yy::location left_position,
			yy::location right_position) const;
	virtual const_shared_ptr<Result> compute(const string& left,
			const string& right, yy::location left_position,
			yy::location right_position) const;
};

#endif /* COMPARISON_EXPRESSION_H_ */
