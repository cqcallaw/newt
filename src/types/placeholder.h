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

#ifndef TYPES_PLACEHOLDER_H_
#define TYPES_PLACEHOLDER_H_

#include <type_definition.h>
#include <assert.h>

class Placeholder: public TypeDefinition {
public:
	Placeholder() {
	}
	virtual ~Placeholder() {
	}

	virtual bool IsSpecifiedBy(const std::string& name,
			const TypeSpecifier& type_specifier) const {
		return false;
	}

	virtual const std::string ToString(const TypeTable& type_table,
			const Indent& indent) const {
		return "<placeholder>";
	}
};

#endif /* TYPES_PLACEHOLDER_H_ */
