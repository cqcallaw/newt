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

#ifndef VARIABLE_H_
#define VARIABLE_H_

#include <symbol.h>
#include <string>
#include "yyltype.h"
#include <symbol_context.h>
#include <assignment_type.h>

class Expression;
class ExecutionContext;
class Result;

class Variable {
public:
	Variable(const string* name, const YYLTYPE location);
	virtual ~Variable();

	virtual const string* ToString(const ExecutionContext* context) const = 0;

	virtual const TypeSpecifier* GetType(
			const ExecutionContext* context) const = 0;

	const string* GetName() const {
		return m_name;
	}

	const YYLTYPE GetLocation() const {
		return m_location;
	}

	virtual const LinkedList<const Error*>* Validate(
			const ExecutionContext* context) const = 0;

	virtual const Result* Evaluate(const ExecutionContext* context) const = 0;

	virtual const LinkedList<const Error*>* AssignValue(
			const ExecutionContext* context, const Expression* expression,
			const AssignmentType op) const = 0;

protected:
	virtual const LinkedList<const Error*>* SetSymbol(
			const ExecutionContext* context, const bool* value) const = 0;

	virtual const LinkedList<const Error*>* SetSymbol(
			const ExecutionContext* context, const int* value) const = 0;

	virtual const LinkedList<const Error*>* SetSymbol(
			const ExecutionContext* context, const double* value) const = 0;

	virtual const LinkedList<const Error*>* SetSymbol(
			const ExecutionContext* context, const string* value) const = 0;

	virtual const LinkedList<const Error*>* SetSymbol(
			const ExecutionContext* context, const Array* value) const = 0;

	virtual const LinkedList<const Error*>* SetSymbol(
			const ExecutionContext* context,
			const CompoundTypeInstance* value) const = 0;

	const LinkedList<const Error*>* ToErrorList(SetResult result) const;

private:
	const string* m_name;
	const YYLTYPE m_location;
};

#endif /* VARIABLE_H_ */
