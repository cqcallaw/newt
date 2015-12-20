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
#include <index.h>

class Expression;
class ExecutionContext;

class ArrayVariable: public Variable {
public:
	ArrayVariable(const_shared_ptr<string> name, yy::location location,
			const IndexList* index_list, yy::location expression_location);
	virtual ~ArrayVariable();

	const IndexList* GetIndexList() const {
		return m_index_list;
	}

	virtual const_shared_ptr<TypeSpecifier> GetType(
			const ExecutionContext* context) const;

	virtual const std::string* ToString(const ExecutionContext* context) const;

	const yy::location GetIndexListLocation() const {
		return m_index_list_location;
	}

	virtual const bool IsBasicReference() const {
		return false;
	}

	virtual const_shared_ptr<TypeSpecifier> GetInnerMostElementType(
			const ExecutionContext* context) const;

	virtual const LinkedList<const Error*>* Validate(
			const ExecutionContext* context) const;

	virtual const Result* Evaluate(const ExecutionContext* context) const;

	virtual const LinkedList<const Error*>* AssignValue(
			const ExecutionContext* context, const Expression* expression,
			const AssignmentType op) const;

protected:
	virtual const LinkedList<const Error*>* SetSymbol(
			const ExecutionContext* context,
			const_shared_ptr<bool> value) const;

	virtual const LinkedList<const Error*>* SetSymbol(
			const ExecutionContext* context, const_shared_ptr<int> value) const;

	virtual const LinkedList<const Error*>* SetSymbol(
			const ExecutionContext* context,
			const_shared_ptr<double> value) const;

	virtual const LinkedList<const Error*>* SetSymbol(
			const ExecutionContext* context,
			const_shared_ptr<string> value) const;

	virtual const LinkedList<const Error*>* SetSymbol(
			const ExecutionContext* context,
			const_shared_ptr<Array> value) const;

	virtual const LinkedList<const Error*>* SetSymbol(
			const ExecutionContext* context,
			const_shared_ptr<CompoundTypeInstance> value) const;

	class ValidationResult {
	public:
		ValidationResult(const_shared_ptr<Array> array, const int index,
				const yy::location index_location,
				const LinkedList<const Error*>* errors) :
				m_array(array), m_index(index), m_index_location(
						index_location), m_errors(errors) {
		}

		const int GetIndex() const {
			return m_index;
		}

		const_shared_ptr<Array> GetArray() const {
			return m_array;
		}

		const LinkedList<const Error*>* GetErrors() const {
			return m_errors;
		}

		const yy::location GetIndexLocation() const {
			return m_index_location;
		}

	private:
		const_shared_ptr<Array> m_array;
		const int m_index;
		const yy::location m_index_location;
		const LinkedList<const Error*>* m_errors;
	};

private:
	const IndexList* m_index_list;
	const yy::location m_index_list_location;

	const ValidationResult* ValidateOperation(
			const ExecutionContext* context) const;

	const LinkedList<const Error*>* SetSymbolCore(
			const ExecutionContext* context,
			const_shared_ptr<void> value) const;
};

#endif /* ARRAY_VARIABLE_H_ */
