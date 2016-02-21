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

#ifndef MATCH_H_
#define MATCH_H_

#include <memory>
#include <linked_list.h>
#include <string>
#include <match.h>

class StatementBlock;

class Match {
public:
	Match(const_shared_ptr<std::string> name, const yy::location name_location,
			const_shared_ptr<StatementBlock> block) :
			m_name(name), m_name_location(name_location), m_block(block) {
	}

	virtual ~Match() {
	}

	const const_shared_ptr<StatementBlock> GetBlock() const {
		return m_block;
	}

	const const_shared_ptr<std::string> GetName() const {
		return m_name;
	}

	const yy::location GetNameLocation() const {
		return m_name_location;
	}

private:
	const_shared_ptr<std::string> m_name;
	const yy::location m_name_location;
	const_shared_ptr<StatementBlock> m_block;
};

typedef const LinkedList<const Match, NO_DUPLICATES> MatchList;
typedef std::shared_ptr<MatchList> MatchListRef;

#endif /* MATCH_H_ */
