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

#ifndef EXPRESSION_H_
#define EXPRESSION_H_

#include <symbol_table.h>
#include <symbol.h>
#include <array_symbol.h>
#include <utils.h>
#include <assert.h>
#include <type.h>
#include <linked_list.h>

class ExecutionContext;
class Error;

class EvaluationResult {
public:
	EvaluationResult(const void* data, const LinkedList<const Error*>* errors) :
			m_data(data), m_errors(errors) {
	}

	const void* GetData() const {
		return m_data;
	}

	const LinkedList<const Error*>* GetErrors() const {
		return m_errors;
	}

private:
	const void* m_data;
	const LinkedList<const Error*>* m_errors;
};

class Expression {
public:
	Expression(const YYLTYPE position);
	virtual ~Expression();

	virtual const Type GetType(
			const ExecutionContext* execution_context) const = 0;

	virtual const EvaluationResult* Evaluate(
			const ExecutionContext* execution_context) const = 0;

	const string* ToString(const ExecutionContext* execution_context) const;

	const YYLTYPE GetPosition() const {
		return m_position;
	}

	virtual const bool IsConstant() const = 0;

	virtual const LinkedList<const Error*>* Validate(
			const ExecutionContext* execution_context) const = 0;

private:
	const YYLTYPE m_position;
};

#endif /* EXPRESSION_H_ */
