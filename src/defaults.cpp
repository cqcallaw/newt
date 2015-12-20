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

#include <defaults.h>

const yy::location GetDefaultLocation() {
	static volatile_shared_ptr<std::string> default_file_name =
			volatile_shared_ptr<std::string>(new std::string(""));
	static yy::location instance = yy::location(default_file_name.get(), -1,
			-1);
	return instance;
}
