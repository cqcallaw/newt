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

#include <sum.h>
#include <sum_type_specifier.h>
#include <sstream>
#include <indent.h>
#include <symbol.h>

Sum::Sum(const_shared_ptr<SumTypeSpecifier> type,
		const_shared_ptr<TypeSpecifier> tag, const_shared_ptr<void> value) :
		m_type(type), m_tag(tag), m_value(value) {
	assert(m_type);
	assert(m_tag);
	assert(m_type != m_tag);
	assert(m_value);
}

Sum::~Sum() {
}

const string Sum::ToString(const TypeTable& type_table,
		const Indent& indent) const {
	ostringstream buffer;
	buffer << Symbol::ToString(m_tag, m_value, type_table, indent);
	buffer << " {" << m_tag->ToString() << "}";
	return buffer.str();
}

const_shared_ptr<Sum> Sum::WithValue(const_shared_ptr<TypeSpecifier> tag,
		const_shared_ptr<void> value) const {
	if (!tag->IsAssignableTo(m_type)) {
		return nullptr;
	}

	return make_shared<Sum>(m_type, tag, value);
}
