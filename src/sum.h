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

#ifndef SUM_H_
#define SUM_H_

#include <sum_type_specifier.h>
#include <type_definition.h>

class Indent;
class SumType;

class Sum {
public:
	Sum(const_shared_ptr<std::string> tag, const_shared_ptr<void> value);
	virtual ~Sum();

	static const_shared_ptr<Sum> GetDefaultInstance(const SumType& type,
			const_shared_ptr<type_specifier_map> type_specifier_mapping);

	const_shared_ptr<void> GetValue() const {
		return m_value;
	}

	const_shared_ptr<std::string> GetTag() const {
		return m_tag;
	}

	const string ToString(const SumType& type, const TypeTable& type_table,
			const Indent& indent,
			const_shared_ptr<type_specifier_map> type_specifier_mapping) const;

private:
	const_shared_ptr<std::string> m_tag;
	const_shared_ptr<void> m_value;
};

#endif /* SUM_H_ */
