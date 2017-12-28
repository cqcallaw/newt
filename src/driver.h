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

#ifndef DRIVER_H_
#define DRIVER_H_

#include "parser.tab.hh"

#define YY_DECL \
	yy::newt_parser::symbol_type yylex (Driver& driver)
YY_DECL;

enum TRACE {
	NO_TRACE = 0, SCANNING = 1, PARSING = 2,
};

class yy_buffer_state;

class InputStackEntry {
public:
	InputStackEntry(FILE* file_handle, yy_buffer_state* buffer_state,
			const yy::location location) :
			m_file_handle(file_handle), m_buffer_state(buffer_state), m_location(
					location) {
	}

	FILE* GetFileHandle() const {
		return m_file_handle;
	}

	yy_buffer_state* GetBufferState() const {
		return m_buffer_state;
	}

	const yy::location GetLocation() const {
		return m_location;
	}

private:
	FILE* m_file_handle;
	yy_buffer_state* m_buffer_state;
	const yy::location m_location;
};

typedef plain_shared_ptr<InputStackEntry> input_entry;
typedef std::stack<input_entry, std::deque<input_entry>> input_stack;
typedef std::vector<plain_shared_ptr<std::string>> string_list;

class Driver {
public:
	Driver() :
			m_input_stack(make_shared<input_stack>()), m_include_file_names(
					make_shared<string_list>()) {
	}

	virtual ~Driver() {
	}

	int scan_begin(volatile_shared_ptr<string>, const bool trace_scanning);
	void scan_end();

	// Run the parser on the file specified by <file_name>
	// Return 0 on success.
	int parse(volatile_shared_ptr<string> file_name, const TRACE trace_level);

	// Run the parser on specified input string
	// Return 0 on success.
	int parse_string(const std::string& string, const TRACE trace_level);

	int scan_string_begin(const std::string& string, const bool trace_scanning);
	void scan_string_end();

	void error(const std::string& message);
	void lexer_error(const yy::location& location, const std::string& message);
	void invalid_token(const yy::location& location,
			const std::string& message);
	void parser_error(const yy::location& location, const std::string& message);

	const plain_shared_ptr<StatementBlock> GetStatementBlock() const {
		return m_statement_block;
	}

	void SetStatementBlock(const_shared_ptr<StatementBlock> statementBlock) {
		m_statement_block = statementBlock;
	}

	unsigned int GetErrorCount() const {
		return m_error_count;
	}

	volatile_shared_ptr<input_stack> GetInputStack() const {
		return m_input_stack;
	}

	volatile_shared_ptr<string_list> GetIncludeFileNames() const {
		return m_include_file_names;
	}

private:
	std::string m_file_name;
	plain_shared_ptr<StatementBlock> m_statement_block;
	unsigned int m_error_count = 0;
	volatile_shared_ptr<input_stack> m_input_stack;
	volatile_shared_ptr<string_list> m_include_file_names;
};

#endif /* DRIVER_H_ */
