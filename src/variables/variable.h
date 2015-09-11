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

#include <string>
#include "symbol.h"
#include "yyltype.h"

class Expression;
class ExecutionContext;

class Variable {
public:
	Variable(const string* name, YYLTYPE location);
	virtual ~Variable();

	virtual const string* ToString() const;

	const static Variable* DefaultVariable;

	virtual const TypeSpecifier* GetType(const ExecutionContext* context) const;

	const string* GetName() const {
		return m_name;
	}

	const YYLTYPE GetLocation() const {
		return m_location;
	}

	virtual const bool IsBasicReference() const {
		return true;
	}

private:
	const string* m_name;
	const YYLTYPE m_location;
};

std::ostream &operator<<(std::ostream &os, const Variable &variable);

#endif /* VARIABLE_H_ */
