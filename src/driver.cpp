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

#include <driver.h>

int Driver::parse(const std::string& file_name, const bool trace_scanning,
		const bool trace_parsing) {
	int scan_begin_result = scan_begin(file_name, trace_scanning);

	if (scan_begin_result != EXIT_SUCCESS)
		return scan_begin_result;

	yy::newt_parser parser(*this);
	parser.set_debug_level(trace_parsing);
	int res = parser.parse();
	scan_end();
	return res;
}

void Driver::error(const std::string& message) {
	std::cerr << message;
}

void Driver::lexer_error(const yy::location& location,
		const std::string& message) {
	std::cerr << location << " Syntax error: " << message << std::endl;
}

void Driver::invalid_token(const yy::location& location,
		const std::string& message) {
	std::cerr << location << " Syntax error: '" << message << "'"
			<< " is not a legal token." << std::endl;
}

void Driver::parser_error(const yy::location& location,
		const std::string& message) {
	Error::parse_error(location.begin.line, location.begin.column, message);
}
