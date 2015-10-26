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

#ifndef FUNCTION_H_
#define FUNCTION_H_

class FunctionTypeSpecifier;
class StatementBlock;
class Result;
class ArgumentList;
class ExecutionContext;

class Function {
public:
	Function(const FunctionTypeSpecifier* type, const StatementBlock* body);
	virtual ~Function();

	const FunctionTypeSpecifier* GetType() const {
		return m_type;
	}

	const Result* Evaluate(const ArgumentList* argument_list,
			const ExecutionContext* execution_context) const;

private:
	const FunctionTypeSpecifier* m_type;
	const StatementBlock* m_body;
};

#endif /* FUNCTION_H_ */
