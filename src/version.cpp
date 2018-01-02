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

#include <version.h>
#include <utils.h>
#include <builtins.h>

Version::~Version() {
}

const Version Version::Build(const std::string& version_string) {
	auto split = Tokenize(version_string, string("."));

	try {
		if (split->size() == 1) {
			return Version(std::stoi(*(split->at(0))), 0, 0);
		} else if (split->size() == 2) {
			return Version(std::stoi(*(split->at(0))),
					std::stoi(*(split->at(1))), 0);
		} else if (split->size() == 3) {
			return Version(std::stoi(*(split->at(0))),
					std::stoi(*(split->at(1))), std::stoi(*(split->at(2))));
		} else {
			// too many or too few version specifiers
			return Version(0, 0, 0);
		}
	} catch (const std::invalid_argument& ia) {
		return Version(0, 0, 0);
	}
}

const Version::Comparison Version::Compare(const Version& other) {
	if (other.m_major > m_major) {
		return Comparison::GREATER;
	} else if (other.m_major > m_major) {
		return Comparison::LESS;
	} else if (other.m_minor > m_minor) {
		return Comparison::GREATER;
	} else if (other.m_minor > m_minor) {
		return Comparison::LESS;
	} else if (other.m_patch > m_patch) {
		return Comparison::GREATER;
	} else if (other.m_patch > m_patch) {
		return Comparison::LESS;
	} else {
		return Comparison::EQUAL;
	}
}

const bool Version::IsCompatible(const Version& other) {
	return other.m_major == m_major && other.m_minor >= m_minor
			&& other.m_patch >= m_patch;
}

const Version Version::GetDefaultVersion() {
	static Version instance = Build(
			*(Tokenize(*Builtins::LANGUAGE_VERSION_STRING)->at(0)));
	return instance;
}

const bool Version::IsVoidVersion() {
	return (m_major == 0 && m_minor == 0 && m_patch == 0);
}

Version::Version(const uint major, const uint minor, const uint patch) :
		m_major(major), m_minor(minor), m_patch(patch) {
}
