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

#ifndef DIMENSION_H_
#define DIMENSION_H_

#include <yyltype.h>

class Dimension {
public:
	Dimension(const YYLTYPE location) :
			m_location(location) {
	}
	virtual ~Dimension() {
	}
private:
	const YYLTYPE m_location;
};

typedef LinkedList<const Dimension*> DimensionList;

#endif /* DIMENSION_H_ */
