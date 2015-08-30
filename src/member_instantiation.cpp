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

#include <member_instantiation.h>

MemberInstantiation::MemberInstantiation(const std::string* name,
		const YYLTYPE name_position, const Expression* expression,
		const YYLTYPE expression_position) :
		m_name(name), m_name_position(name_position), m_expression(expression), m_expression_position(
				expression_position) {
}

MemberInstantiation::~MemberInstantiation() {
}

