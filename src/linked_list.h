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

#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_

#include <stdexcept>
#include <memory>
#include <assert.h>

using namespace std;

enum ListDuplicatePolicy {
	NO_DUPLICATES = 0, ALLOW_DUPLICATES = 1
};

template<class T, ListDuplicatePolicy duplicate_policy> class LinkedList {
public:
	virtual ~LinkedList() {
	}

	const shared_ptr<T> GetData() const {
		return m_data;
	}

	const shared_ptr<const LinkedList<T, duplicate_policy>> GetNext() const {
		return m_next;
	}

	static const shared_ptr<const LinkedList<T, duplicate_policy>> From(
			const shared_ptr<const LinkedList<T, duplicate_policy>> subject) {
		return shared_ptr<const LinkedList<T, duplicate_policy>>(
				new const LinkedList<T, duplicate_policy>(subject));
	}

	static const shared_ptr<const LinkedList<T, duplicate_policy>> From(
			const shared_ptr<T> data,
			const shared_ptr<const LinkedList<T, duplicate_policy>> next) {
		return shared_ptr<const LinkedList<T, duplicate_policy>>(
				new const LinkedList<T, duplicate_policy>(data, next));
	}

	static const shared_ptr<const LinkedList<T, duplicate_policy>> Concatenate(
			const shared_ptr<const LinkedList<T, duplicate_policy>> base,
			const shared_ptr<const LinkedList<T, duplicate_policy>> continuation) {
		if (IsTerminator(base)) {
			return continuation;
		}
		if (IsTerminator(continuation)) {
			return base;
		}

		if (base == continuation) {
			throw std::invalid_argument(
					"Cannot concatenate a list onto itself.");
		}

		shared_ptr<const LinkedList<T, duplicate_policy>> result = base;
		shared_ptr<const LinkedList<T, duplicate_policy>> subject = Reverse(
				continuation);

		while (!IsTerminator(subject)) {
			result = shared_ptr<const LinkedList<T, duplicate_policy>>(
					new const LinkedList<T, duplicate_policy>(
							subject->GetData(), result));
			subject = subject->GetNext();
		}

		return result;
	}

	static const shared_ptr<const LinkedList<T, duplicate_policy>> Reverse(
			const shared_ptr<const LinkedList<T, duplicate_policy>> subject) {
		if (IsTerminator(subject)) {
			return subject;
		}

		//ref: http://stackoverflow.com/a/1801703/577298
		shared_ptr<const LinkedList<T, duplicate_policy>> iter = subject;
		shared_ptr<const LinkedList<T, duplicate_policy>> new_next =
				GetTerminator();
		while (!IsTerminator(iter)) {
			new_next = shared_ptr<const LinkedList<T, duplicate_policy>>(
					new const LinkedList<T, duplicate_policy>(iter->GetData(),
							new_next));
			iter = iter->GetNext();
		}

		return new_next;
	}

	static const bool IsTerminator(
			const shared_ptr<const LinkedList<T, duplicate_policy>> subject) {
		return subject == GetTerminator();
	}

	static const shared_ptr<const LinkedList<T, duplicate_policy>> GetTerminator() {
		static const shared_ptr<const LinkedList<T, duplicate_policy>> terminator;
		return terminator;
	}

private:
	LinkedList(const shared_ptr<T> data) :
			LinkedList(data, GetTerminator()) {
	}

	LinkedList(const shared_ptr<T> data,
			const shared_ptr<const LinkedList<T, duplicate_policy>> next) :
			m_data(data), m_next(next) {
		if (duplicate_policy == NO_DUPLICATES && !IsTerminator(m_next)) {
			assert(m_data != m_next->GetData());
		}
	}

//construct a linked list from the given input
//map null inputs to empty lists
	LinkedList(const shared_ptr<const LinkedList<T, duplicate_policy>> list) :
			m_data(
					(list.get() != nullptr && !IsTerminator(list)) ?
							list->GetData() : nullptr), m_next(
					(list.get() != nullptr && !IsTerminator(list)) ?
							list->GetNext() : GetTerminator()) {
	}

	const shared_ptr<T> m_data;
	const shared_ptr<const LinkedList<T, duplicate_policy>> m_next;
};

#endif /* LINKED_LIST_H_ */
