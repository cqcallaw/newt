/*
 * linked_list.h
 *
 *  Created on: Mar 31, 2015
 *      Author: caleb
 */

#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_

template<class T> class LinkedList {
public:
	LinkedList(const T data, const LinkedList<T>* next) :
			m_data(data), m_next(next) {
	}

	//construct a linked list from the given input
	//map null inputs to empty lists
	LinkedList(const LinkedList<T>* list) :
			m_data(
					(list != nullptr && list != Terminator) ?
							list->GetData() : nullptr), m_next(
					(list != nullptr && list != Terminator) ?
							list->GetNext() : Terminator) {
	}

	virtual ~LinkedList() {
	}

	const T GetData() const {
		return m_data;
	}

	const LinkedList<T>* GetNext() const {
		return m_next;
	}

	const LinkedList<T>* Reverse(bool delete_original) const {
		//ref: http://stackoverflow.com/a/1801703/577298
		LinkedList<T>* subject = (LinkedList<T>*) this;
		LinkedList<T>* new_next = (LinkedList<T>*) Terminator;
		while (subject != Terminator) {
			new_next = new LinkedList<T>(subject->GetData(), new_next);
			LinkedList<T>* tmp = subject;
			subject = (LinkedList<T>*) subject->GetNext();
			if (delete_original) {
				delete (tmp);
			}
		}

		return new_next;
	}

	static constexpr LinkedList<T>* Terminator = nullptr;

private:
	const T m_data;
	const LinkedList<T>* m_next;
};

#endif /* LINKED_LIST_H_ */
