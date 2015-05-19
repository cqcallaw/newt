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

 The standard assert library uses macros to implements asserts.

 While this is efficient, it is hard to debug.  You cannot set a breakpoint
 in the debugger for standard assert statements.

 This implementation uses a function to implement assert statements

 If you use this implementation you can set break points in the assert function
 */

#undef assert

#ifdef NDEBUG           /* required by ANSI standard */
#define assert(p)          ((void)0)
#else
#define assert(e)       ((e) ? (void)0 : __pretty_assert(__FILE__, __LINE__, #e))

#endif /* NDEBUG */

void __pretty_assert(const char *, int, const char *);

