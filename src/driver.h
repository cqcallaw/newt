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

class Driver {
public:
	Driver() {
	}
	virtual ~Driver() {
	}

	int scan_begin(const std::string& file_name, const bool trace_scanning);
	void scan_end();

	// Run the parser on the file specified by <file_name>
	// Return 0 on success.
	int parse(const std::string& file_name, const bool trace_parsing,
			const bool trace_scanning);

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

private:
	std::string m_file_name;
	plain_shared_ptr<StatementBlock> m_statement_block;
	unsigned int m_error_count = 0;
};

#endif /* DRIVER_H_ */
