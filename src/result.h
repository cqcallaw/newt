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

#ifndef RESULT_H_
#define RESULT_H_

#include <error.h>
#include <symbol.h>

class Result {
public:
	Result(const_shared_ptr<void> data, const ErrorListRef errors) :
			m_data(data), m_errors(errors) {
		assert(m_data || m_errors);
	}

	template<class T> const shared_ptr<const T> GetData() const {
		if (m_data && ErrorList::IsTerminator(m_errors)) {
			auto cast = static_pointer_cast<const T>(m_data);
			return cast;
		}

		return shared_ptr<const T>();
	}

	const ErrorListRef GetErrors() const {
		return m_errors;
	}

	/**
	 * Get the raw void pointer to the data.
	 *
	 * This should be used judiciously; the templated GetData member is almost always a better option.
	 */
	const_shared_ptr<void> GetRawData() const {
		return m_data;
	}

private:
	const_shared_ptr<void> m_data;
	const ErrorListRef m_errors;
};

template<class T> class TypedResult {
public:
	TypedResult(const_shared_ptr<T> data) :
			TypedResult(data, ErrorList::GetTerminator()) {
	}

	TypedResult(const_shared_ptr<T> data, const ErrorListRef errors) :
			m_data(data), m_errors(errors) {
		assert(m_data || m_errors);
	}

	const shared_ptr<const T> GetData() const {
		return m_data;
	}

	const ErrorListRef GetErrors() const {
		return m_errors;
	}

private:
	const_shared_ptr<T> m_data;
	const ErrorListRef m_errors;
};

class PreprocessResult {
public:
	enum ReturnCoverage {
		NONE, PARTIAL, FULL
	};

	PreprocessResult(ReturnCoverage return_coverage, const ErrorListRef errors) :
			m_return_coverage(return_coverage), m_errors(errors) {

	}

	virtual ~PreprocessResult() {
	}

	const ReturnCoverage GetReturnCoverage() const {
		return m_return_coverage;
	}

	const ErrorListRef GetErrors() const {
		return m_errors;
	}

private:
	const ReturnCoverage m_return_coverage;
	const ErrorListRef m_errors;
};

class ExecutionResult {
public:
	ExecutionResult() :
			ExecutionResult(Symbol::GetDefaultSymbol(), GetDefaultExitCode(),
					ErrorList::GetTerminator()) {
	}
	ExecutionResult(const_shared_ptr<Symbol> return_value) :
			ExecutionResult(return_value, GetDefaultExitCode(),
					ErrorList::GetTerminator()) {
	}
	ExecutionResult(const_shared_ptr<int> exit_code) :
			ExecutionResult(Symbol::GetDefaultSymbol(), exit_code,
					ErrorList::GetTerminator()) {
	}
	ExecutionResult(const ErrorListRef errors) :
			ExecutionResult(Symbol::GetDefaultSymbol(), GetDefaultExitCode(),
					errors) {
	}

	virtual ~ExecutionResult() {
	}

	const bool NeedsReturn() const {
		return m_return_value
				!= Symbol::GetDefaultSymbol()
				|| m_exit_code
						!= ExecutionResult::GetDefaultExitCode();
	}

	const ErrorListRef GetErrors() const {
		return m_errors;
	}

	const const_shared_ptr<int> GetExitCode() const {
		return m_exit_code;
	}

	const_shared_ptr<Symbol> GetReturnValue() const {
		return m_return_value;
	}

	static plain_shared_ptr<int> GetDefaultExitCode() {
		return nullptr;
	}

private:
	ExecutionResult(const_shared_ptr<Symbol> return_value,
			const_shared_ptr<int> exit_code, const ErrorListRef errors) :
			m_return_value(return_value), m_exit_code(exit_code), m_errors(
					errors) {

	}

	const_shared_ptr<Symbol> m_return_value;
	const_shared_ptr<int> m_exit_code;
	const ErrorListRef m_errors;
};

#endif /* RESULT_H_ */
