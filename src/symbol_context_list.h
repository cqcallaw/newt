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

#ifndef SYMBOL_CONTEXT_LIST_H_
#define SYMBOL_CONTEXT_LIST_H_

using namespace std;

#include <assert.h>
#include <execution_context.h>

class SymbolContextList {
public:

	virtual ~SymbolContextList() {
	}

	shared_ptr<SymbolContext> GetData() const {
		if (m_data) {
			return m_data;
		} else {
			return m_weak_data.lock();
		}
	}

	bool IsWeak() const {
		return !m_data;
	}

	shared_ptr<SymbolContextList> GetNext() const {
		return m_next;
	}

	static shared_ptr<SymbolContextList> From(
			const_shared_ptr<ExecutionContext> context,
			const shared_ptr<SymbolContextList> context_parent) {
		shared_ptr<SymbolContext> symbol_context = context->GetSymbolContext();

		if (context->GetLifeTime() == PERSISTENT) {
			weak_ptr<SymbolContext> weak = symbol_context;
			return shared_ptr<SymbolContextList>(
					new SymbolContextList(weak, context_parent));
		} else {
			return shared_ptr<SymbolContextList>(
					new SymbolContextList(symbol_context, context_parent));
		}
	}


	static const bool IsTerminator(shared_ptr<SymbolContextList> subject) {
		return subject == GetTerminator();
	}

	static shared_ptr<SymbolContextList> GetTerminator() {
		static shared_ptr<SymbolContextList> terminator;
		return terminator;
	}

private:
	SymbolContextList(const shared_ptr<SymbolContext> data) :
			SymbolContextList(data, GetTerminator()) {
	}

	SymbolContextList(const shared_ptr<SymbolContext> data,
			const shared_ptr<SymbolContextList> next) :
			m_data(data), m_weak_data(shared_ptr<SymbolContext>(nullptr)), m_next(
					next) {
		if (m_next) {
			assert(m_data != m_next->GetData());
		}
	}

	SymbolContextList(const weak_ptr<SymbolContext> data) :
			SymbolContextList(data, GetTerminator()) {
	}

	SymbolContextList(const weak_ptr<SymbolContext> data,
			const shared_ptr<SymbolContextList> next) :
			m_data(nullptr), m_weak_data(data), m_next(next) {
		if (m_next) {
			assert(m_weak_data.lock() != m_next->GetData());
		}
	}

	shared_ptr<SymbolContext> m_data;
	weak_ptr<SymbolContext> m_weak_data;
	shared_ptr<SymbolContextList> m_next;
};

#endif /* SYMBOL_CONTEXT_LIST_H_ */
