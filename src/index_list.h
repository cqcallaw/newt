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

#ifndef INDEX_LIST_H_
#define INDEX_LIST_H_

class Index;

class IndexList: public LinkedList<const Index*> {
public:
	IndexList(const Index* data, const IndexList* next) :
			LinkedList(data, next) {
	}

	IndexList(const LinkedList<const Index*>* list) :
			LinkedList(list) {
	}

	const static IndexList* GetTerminator() {
		static IndexList* terminator =
				(IndexList*) LinkedList<const Index*>::GetTerminator();
		return terminator;
	}
};

#endif /* INDEX_LIST_H_ */
