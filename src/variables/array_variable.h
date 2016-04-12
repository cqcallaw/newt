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
#include <assignment_type.h>

class Expression;
class ExecutionContext;

class ArrayVariable: public Variable {
public:
	ArrayVariable(const_shared_ptr<Variable> base_variable,
			const_shared_ptr<Expression> expression);
	virtual ~ArrayVariable();

	virtual const_shared_ptr<TypeSpecifier> GetTypeSpecifier(
			const shared_ptr<ExecutionContext> context,
			AliasResolution resolution = AliasResolution::RESOLVE) const;

	virtual const_shared_ptr<string> ToString(
			const shared_ptr<ExecutionContext> context) const;

	virtual const bool IsBasicReference() const {
		return false;
	}

//	virtual const_shared_ptr<TypeSpecifier> GetInnerMostElementType(
//			const shared_ptr<ExecutionContext> context) const;

	virtual const_shared_ptr<TypeSpecifier> GetElementType(
			const shared_ptr<ExecutionContext> context) const;

	virtual const ErrorListRef Validate(
			const shared_ptr<ExecutionContext> context) const;

	virtual const_shared_ptr<Result> Evaluate(
			const shared_ptr<ExecutionContext> context) const;

	virtual const ErrorListRef AssignValue(
			const shared_ptr<ExecutionContext> context,
			const_shared_ptr<Expression> expression,
			const AssignmentType op) const;

protected:
	virtual const ErrorListRef SetSymbol(
			const shared_ptr<ExecutionContext> context,
			const_shared_ptr<bool> value) const;

	virtual const ErrorListRef SetSymbol(
			const shared_ptr<ExecutionContext> context,
			const_shared_ptr<int> value) const;

	virtual const ErrorListRef SetSymbol(
			const shared_ptr<ExecutionContext> context,
			const_shared_ptr<double> value) const;

	virtual const ErrorListRef SetSymbol(
			const shared_ptr<ExecutionContext> context,
			const_shared_ptr<string> value) const;

	virtual const ErrorListRef SetSymbol(
			const shared_ptr<ExecutionContext> context,
			const_shared_ptr<Array> value) const;

	virtual const ErrorListRef SetSymbol(
			const shared_ptr<ExecutionContext> context,
			const_shared_ptr<ComplexTypeSpecifier> type,
			const_shared_ptr<Record> value) const;

	class ValidationResult {
	public:
		ValidationResult(const_shared_ptr<Array> array, const int index,
				const yy::location index_location, const ErrorListRef errors) :
				m_array(array), m_index(index), m_index_location(
						index_location), m_errors(errors) {
		}

		const int GetIndex() const {
			return m_index;
		}

		const_shared_ptr<Array> GetArray() const {
			return m_array;
		}

		const ErrorListRef GetErrors() const {
			return m_errors;
		}

		const yy::location GetIndexLocation() const {
			return m_index_location;
		}

	private:
		const_shared_ptr<Array> m_array;
		const int m_index;
		const yy::location m_index_location;
		const ErrorListRef m_errors;
	};

private:
	const_shared_ptr<Variable> m_base_variable;
	const_shared_ptr<Expression> m_expression;

	const_shared_ptr<ArrayVariable::ValidationResult> ValidateOperation(
			const shared_ptr<ExecutionContext> context) const;

	const ErrorListRef SetSymbolCore(const shared_ptr<ExecutionContext> context,
			const_shared_ptr<void> value) const;
};

#endif /* ARRAY_VARIABLE_H_ */
