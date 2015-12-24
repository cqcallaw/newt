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

#include <defaults.h>

class Dimension {
public:
	Dimension(const yy::location location) :
			m_location(location) {
	}
	virtual ~Dimension() {
	}
private:
	const yy::location m_location;
};

typedef const LinkedList<const Dimension, NO_DUPLICATES> DimensionListBase;
typedef shared_ptr<DimensionListBase> DimensionList;

#endif /* DIMENSION_H_ */
