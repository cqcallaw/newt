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

template<class T> class LinkedList {
public:
	LinkedList(const T data) :
			m_data(data), m_next(GetTerminator()) {
	}

	LinkedList(const T data, const LinkedList<T>* next) :
			m_data(data), m_next(next) {
	}

	//construct a linked list from the given input
	//map null inputs to empty lists
	LinkedList(const LinkedList<T>* list) :
			m_data(
					(list != nullptr && list != GetTerminator()) ?
							list->GetData() : nullptr), m_next(
					(list != nullptr && list != GetTerminator()) ?
							list->GetNext() : GetTerminator()) {
	}

	virtual ~LinkedList() {
	}

	const T GetData() const {
		return m_data;
	}

	const LinkedList<T>* GetNext() const {
		return m_next;
	}

	const LinkedList<T>* With(const T data) const {
		return new LinkedList<T>(data, this);
	}

	const LinkedList<T>* Concatenate(const LinkedList<T>* other,
			bool delete_original) const {
		if (this->IsTerminator()) {
			return other;
		}
		if (other->IsTerminator()) {
			return this;
		}

		if (this == other) {
			throw std::invalid_argument(
					"Cannot concatenate a list onto itself.");
		}

		auto result = this;
		auto subject = other->Reverse(delete_original);

		while (!subject->IsTerminator()) {
			result = new LinkedList<T>(subject->GetData(), result);
			subject = subject->GetNext();
		}

		return result;
	}

	const LinkedList<T>* Reverse(bool delete_original) const {
		//ref: http://stackoverflow.com/a/1801703/577298
		const LinkedList<T>* subject = this;
		const LinkedList<T>* new_next = GetTerminator();
		while (subject != GetTerminator()) {
			new_next = new LinkedList<T>(subject->GetData(), new_next);
			const LinkedList<T>* tmp = subject;
			subject = subject->GetNext();
			if (delete_original) {
				delete (tmp);
			}
		}

		return new_next;
	}

	const bool IsTerminator() const {
		return this == GetTerminator();
	}

	static const LinkedList<T>* GetTerminator() {
		static LinkedList<T>* terminator = nullptr;
		return terminator;
	}

private:
	const T m_data;
	const LinkedList<T>* m_next;
};

#endif /* LINKED_LIST_H_ */
