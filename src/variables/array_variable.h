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

#ifndef ARRAY_VARIABLE_H_
#define ARRAY_VARIABLE_H_

#include <string>
#include "variable.h"

class Expression;
class ExecutionContext;

class ArrayVariable: public Variable {
public:
	ArrayVariable(const std::string* name, YYLTYPE location,
			const Expression* index_expression, YYLTYPE expression_location);
	virtual ~ArrayVariable();

	const Expression* GetIndexExpression() const {
		return m_index_expression;
	}

	virtual const TypeSpecifier* GetType(const ExecutionContext* context) const;

	virtual const std::string* ToString(const ExecutionContext* context) const;

	const YYLTYPE GetExpressionLocation() const {
		return m_expression_location;
	}

	virtual const bool IsBasicReference() const {
		return false;
	}

	virtual const Result* Evaluate(const ExecutionContext* context) const;

	virtual const LinkedList<const Error*>* SetSymbol(
			const ExecutionContext* context, const bool* value) const;

	virtual const LinkedList<const Error*>* SetSymbol(
			const ExecutionContext* context, const int* value) const;

	virtual const LinkedList<const Error*>* SetSymbol(
			const ExecutionContext* context, const double* value) const;

	virtual const LinkedList<const Error*>* SetSymbol(
			const ExecutionContext* context, const string* value) const;

	virtual const LinkedList<const Error*>* SetSymbol(
			const ExecutionContext* context,
			const CompoundTypeInstance* value) const;

	virtual const LinkedList<const Error*>* Validate(
			const ExecutionContext* context) const;

protected:
	class ValidationResult {
	public:
		ValidationResult(const ArraySymbol* symbol, const int index,
				const LinkedList<const Error*>* errors) :
				m_symbol(symbol), m_index(index), m_errors(errors) {
		}

		const int GetIndex() const {
			return m_index;
		}

		const ArraySymbol* GetSymbol() const {
			return m_symbol;
		}

		const LinkedList<const Error*>* GetErrors() const {
			return m_errors;
		}
	private:
		const ArraySymbol* m_symbol;
		const int m_index;
		const LinkedList<const Error*>* m_errors;
	};

private:
	const Expression* m_index_expression;
	const YYLTYPE m_expression_location;

	const ValidationResult* ValidateOperation(const ExecutionContext* context) const;

	const LinkedList<const Error*>* SetSymbolCore(
			const ExecutionContext* context, const void* value) const;
};

#endif /* ARRAY_VARIABLE_H_ */
