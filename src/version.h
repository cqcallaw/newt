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

#ifndef VERSION_H_
#define VERSION_H_

#include <string>

class Version {
public:
	enum Comparison {
		INCOMPARABLE = 0, LESS = 1, EQUAL = 2, GREATER = 3
	};

	static const Version Build(const std::string& version_string);
	static const Version GetDefaultVersion();
	virtual ~Version();

	const Comparison Compare(const Version& other);

	const bool IsCompatible(const Version& other);

	const bool IsVoidVersion();

private:
	Version(const uint major, const uint minor, const uint patch);
	const uint m_major;
	const uint m_minor;
	const uint m_patch;
};

#endif /* VERSION_H_ */
