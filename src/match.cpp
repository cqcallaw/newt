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

#include <match.h>

const_shared_ptr<std::string> Match::DEFAULT_MATCH_NAME = make_shared<
		std::string>("_");

Match::Match(const_shared_ptr<std::string> name,
		const yy::location name_location, const_shared_ptr<std::string> alias,
		const yy::location alias_location,
		const_shared_ptr<StatementBlock> block) :
		m_name(name), m_name_location(name_location), m_alias(alias), m_alias_location(
				alias_location), m_block(block) {
}

Match::~Match() {
}
