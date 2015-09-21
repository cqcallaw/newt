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

#ifndef VARIABLES_BASIC_VARIABLE_H_
#define VARIABLES_BASIC_VARIABLE_H_

#include <variable.h>

class BasicVariable: public Variable {
public:
	BasicVariable(const string* name, const YYLTYPE location);
	virtual ~BasicVariable();

	virtual const string* ToString(const ExecutionContext* context) const;

	const static Variable* GetDefaultVariable();

	virtual const TypeSpecifier* GetType(const ExecutionContext* context) const;

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
};

#endif /* VARIABLES_BASIC_VARIABLE_H_ */
