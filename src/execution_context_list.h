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

#ifndef EXECUTION_CONTEXT_LIST_H_
#define EXECUTION_CONTEXT_LIST_H_

using namespace std;

#include <assert.h>
#include <memory>

using namespace std;

class ExecutionContext;

class ExecutionContextList {
public:
	virtual ~ExecutionContextList() {
	}

	shared_ptr<ExecutionContext> GetData() const {
		if (m_data) {
			return m_data;
		} else {
			return m_weak_data.lock();
		}
	}

	bool IsWeak() const {
		return !m_data;
	}

	shared_ptr<ExecutionContextList> GetNext() const {
		return m_next;
	}

	static shared_ptr<ExecutionContextList> From(
			const shared_ptr<ExecutionContext> context,
			const shared_ptr<ExecutionContextList> context_parent);

	static const bool IsTerminator(shared_ptr<ExecutionContextList> subject) {
		return subject == GetTerminator();
	}

	static shared_ptr<ExecutionContextList> GetTerminator() {
		static shared_ptr<ExecutionContextList> terminator;
		return terminator;
	}

private:
	ExecutionContextList(const shared_ptr<ExecutionContext> data) :
			ExecutionContextList(data, GetTerminator()) {
	}

	ExecutionContextList(const shared_ptr<ExecutionContext> data,
			const shared_ptr<ExecutionContextList> next) :
			m_data(data), m_weak_data(shared_ptr<ExecutionContext>(nullptr)), m_next(
					next) {
		if (m_next) {
			assert(m_data != m_next->GetData());
		}
	}

	ExecutionContextList(const weak_ptr<ExecutionContext> data) :
			ExecutionContextList(data, GetTerminator()) {
	}

	ExecutionContextList(const weak_ptr<ExecutionContext> data,
			const shared_ptr<ExecutionContextList> next) :
			m_data(nullptr), m_weak_data(data), m_next(next) {
		if (m_next) {
			assert(m_weak_data.lock() != m_next->GetData());
		}
	}

	shared_ptr<ExecutionContext> m_data;
	weak_ptr<ExecutionContext> m_weak_data;
	shared_ptr<ExecutionContextList> m_next;
};

#endif /* EXECUTION_CONTEXT_LIST_H_ */
