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

int Driver::parse(volatile_shared_ptr<string> file_name) {
	int scan_begin_result = scan_begin(file_name,
			(m_trace_level & SCANNING) == SCANNING);

	if (scan_begin_result != EXIT_SUCCESS)
		return scan_begin_result;

	yy::newt_parser parser(*this);
	parser.set_debug_level((m_trace_level & PARSING));
	int res = parser.parse();
	scan_end();
	return res;
}

int Driver::parse_string(const std::string& string) {
	int scan_begin_result = scan_string_begin(string,
			(m_trace_level & SCANNING) == SCANNING);

	if (scan_begin_result != EXIT_SUCCESS)
		return scan_begin_result;

	yy::newt_parser parser(*this);
	parser.set_debug_level((m_trace_level & PARSING));
	int res = parser.parse();
	scan_string_end();
	return res;
}

void Driver::error(const std::string& message) {
	std::cerr << message;
	m_error_count++;
}

void Driver::lexer_error(const yy::location& location,
		const std::string& message) {
	std::cerr << "Lex error at " << location << ": " << message << std::endl;
	m_error_count++;
}

void Driver::invalid_token(const yy::location& location,
		const std::string& token_name) {
	std::cerr << "Lex error at " << location << ": '" << token_name << "'"
			<< " is not a legal token." << std::endl;
	m_error_count++;
}

void Driver::parser_error(const yy::location& location,
		const std::string& message) {
	std::cerr << "Parse error at " << location << ": " << message << "."
			<< std::endl;
	m_error_count++;
}
