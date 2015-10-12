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

#include <sstream>
#include <defaults.h>
#include <execution_context.h>
#include <symbol_context.h>

#include "variable.h"
#include "assert.h"
#include "expression.h"

Variable::Variable(const string* name, YYLTYPE location) :
		m_name(name), m_location(location) {
	assert(name != NULL && name != nullptr);
}

Variable::~Variable() {
}

const LinkedList<const Error*>* Variable::ToErrorList(SetResult result) const {
	return ::ToErrorList(result, GetLocation(), GetName());
}
