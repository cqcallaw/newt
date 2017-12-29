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

#ifndef DEFAULTS_H_
#define DEFAULTS_H_

#include <memory>

template<class T>
using const_shared_ptr = const std::shared_ptr<const T>;

template<class T>
using plain_shared_ptr = std::shared_ptr<const T>;

template<class T>
using volatile_shared_ptr = std::shared_ptr<T>;

#include <location.hh>
const yy::location GetDefaultLocation();

typedef std::vector<plain_shared_ptr<std::string>> string_list;

#endif /* DEFAULTS_H_ */
