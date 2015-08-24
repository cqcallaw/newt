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

class Error;

class Result {
public:
	Result(const void* data, const LinkedList<const Error*>* errors) :
			m_data(data), m_errors(errors) {
	}

	const void* GetData() const {
		return m_data;
	}

	const LinkedList<const Error*>* GetErrors() const {
		return m_errors;
	}

private:
	const void* m_data;
	const LinkedList<const Error*>* m_errors;
};

#endif /* RESULT_H_ */
