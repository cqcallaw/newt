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

%{ /* -*- C++ -*- */
#include <cerrno>
#include <climits>
#include <cfloat>
#include <string.h>
#include <regex>

#include <driver.h>
#include <builtins.h>
#include <version.h>
#include "parser.tab.hh"

// The location of the current token.
static yy::location loc;
%}

%option outfile="lexer.cpp"
%option header-file="lexer.h"

%option noyywrap
%option nounput batch debug noinput

id      [_a-zA-Z][a-zA-Z_0-9]*
byte    0x([0-9A-F]|([0-9A-F][0-9A-F]))
int     [0-9]+
double  ([0-9]+\.[0-9]+|[0-9]+\.|\.[0-9]+)
str     \"(\\.|[^\"])*\"
blank   [ \t\r]

%{
  #define YY_USER_ACTION  loc.columns (yyleng);
%}

%x import
%x target

%%

%{
  // Code run each time yylex is called.
  loc.step();
%}

\$target BEGIN(target);
<target>{blank}+ //swallow whitespace
<target>[^ \t\r\n]+ {
	auto version_string = string(yytext);
	if (std::regex_match (version_string, std::regex("(\\d+)\\.(\\d+)(\\.\\d+)?") ))
	{
		auto parsed_version = Version::Build(version_string);
		if (parsed_version.IsVoidVersion()) {
			driver.lexer_error(loc, "Invalid script target '" + string(yytext) + "'.");
		} else if (!parsed_version.IsCompatible(Version::GetDefaultVersion())) {
			driver.lexer_error(loc, "Unsupported script target '" + string(yytext) + "'.");
		}
	} else {
		driver.lexer_error(loc, "Invalid script target '" + string(yytext) + "'.");
	}

	BEGIN(INITIAL);
}

{blank}+   loc.step();
[\n]+      loc.lines(yyleng); loc.step();
\/\/.+$         // ignore comments
#.*$            // ignore comments

import          BEGIN(import);
<import>{blank}+  // swallow whitespace
<import>[^ \t\r\n]+ {
	// got the include file name
	// ref: http://westes.github.io/flex/manual/Multiple-Input-Buffers.html

	auto path_separator = Builtins::get_path_separator();
	auto as_string = string(yytext);
	std::replace(as_string.begin(), as_string.end(), '.', path_separator);
	as_string = as_string + ".nwt";

	auto debug_import = (driver.GetTraceLevel() & IMPORT) == IMPORT;

	if (debug_import) {
		cout << "Normalized import source: " << as_string << endl;
	}

	auto resolved = false;
	for (auto & include_path : *driver.GetIncludePaths()) {
		auto potential_source = *include_path + path_separator + as_string;
		if (FILE *potential_source_file_handle = fopen(potential_source.c_str(), "r")) {
			if (debug_import) {
				cout << "Found import source " << potential_source << endl;
			}
			resolved = true;

			auto included = false;
			for (auto & included_file : *driver.GetIncludedFileNames()) {
				if (strcmp(potential_source.c_str(), included_file->c_str()) == 0) {
					// we have already included the specified file
					if (debug_import) {
						cout << "Source already imported." << endl;
					}
					included = true;
				}
			}

			if (!included) {
				driver.GetInputStack()->push(make_shared<InputStackEntry>(yyin, YY_CURRENT_BUFFER, loc));
	
				yyin = potential_source_file_handle;
				yy_switch_to_buffer(yy_create_buffer(potential_source_file_handle, YY_BUF_SIZE));
				// make sure we have a live reference to the include name as long as the driver is active
				// this will also properly dispose of the include file name memory when the driver is destroyed
				auto source = make_shared<string>(potential_source);
				driver.GetIncludedFileNames()->push_back(source);
				loc = yy::location(source.get(), 1, 1);
			}
		}
	}

	if (!resolved) {
		driver.lexer_error(loc, "Could not resolve import source '" + string(yytext) + "'.");
	}

	BEGIN(INITIAL); // reset start condition so we don't repeat this block indefinitely
}

"bool"          return yy::newt_parser::make_BOOLEAN(loc);
"byte"          return yy::newt_parser::make_BYTE(loc);
"int"           return yy::newt_parser::make_INT(loc);
"double"        return yy::newt_parser::make_DOUBLE(loc);
"string"        return yy::newt_parser::make_STRING(loc);

"if"            return yy::newt_parser::make_IF(loc);
"else"          return yy::newt_parser::make_ELSE(loc);
"for"           return yy::newt_parser::make_FOR(loc);
"while"         return yy::newt_parser::make_WHILE(loc);
"do"            return yy::newt_parser::make_DO(loc);

"exit"          return yy::newt_parser::make_EXIT(loc);
"print"         return yy::newt_parser::make_PRINT(loc);
"="             return yy::newt_parser::make_EQUALS(loc);
"{"             return yy::newt_parser::make_LBRACE(loc);
"}"             return yy::newt_parser::make_RBRACE(loc);
"::"            return yy::newt_parser::make_DOUBLE_COLON(loc);
":"             return yy::newt_parser::make_COLON(loc);
";"             return yy::newt_parser::make_SEMICOLON(loc);
","             return yy::newt_parser::make_COMMA(loc);
"@"             return yy::newt_parser::make_AT(loc);
"?"             return yy::newt_parser::make_QMARK(loc);

"("             return yy::newt_parser::make_LPAREN(loc);
")"             return yy::newt_parser::make_RPAREN(loc);
"["             return yy::newt_parser::make_LBRACKET(loc);
"]"             return yy::newt_parser::make_RBRACKET(loc);
"."             return yy::newt_parser::make_PERIOD(loc);
"+="            return yy::newt_parser::make_PLUS_ASSIGN(loc);
"-="            return yy::newt_parser::make_MINUS_ASSIGN(loc);
"->"            return yy::newt_parser::make_ARROW_RIGHT(loc);

"*"             return yy::newt_parser::make_ASTERISK(loc);
"/"             return yy::newt_parser::make_DIVIDE(loc);
"+"             return yy::newt_parser::make_PLUS(loc);
"-"             return yy::newt_parser::make_MINUS(loc);
"%"             return yy::newt_parser::make_PERCENT(loc);
"<"             return yy::newt_parser::make_LESS(loc);
">"             return yy::newt_parser::make_GREATER(loc);
"<="            return yy::newt_parser::make_LESS_EQUAL(loc);
">="            return yy::newt_parser::make_GREATER_EQUAL(loc);
"=="            return yy::newt_parser::make_EQUAL(loc);
"!="            return yy::newt_parser::make_NOT_EQUAL(loc);
"!"             return yy::newt_parser::make_NOT(loc);
"&&"            return yy::newt_parser::make_AND(loc);
"||"            return yy::newt_parser::make_OR(loc);
"|"             return yy::newt_parser::make_PIPE(loc);
"&"             return yy::newt_parser::make_AMPERSAND(loc);
"mutable"       return yy::newt_parser::make_MUTABLE(loc);
"with"          return yy::newt_parser::make_WITH(loc);
"return"        return yy::newt_parser::make_RETURN(loc);
"match"         return yy::newt_parser::make_MATCH(loc);
"as"            return yy::newt_parser::make_AS(loc);
"in"            return yy::newt_parser::make_IN(loc);
"using"         return yy::newt_parser::make_USING(loc);

"true"          return yy::newt_parser::make_TRUE(true, loc);
"false"         return yy::newt_parser::make_FALSE(false, loc);

"_"             return yy::newt_parser::make_UNDERSCORE(loc);

{byte} {
	uint len = strlen(yytext);

	std::uint8_t n = 0;

	if (len == 4) {
		char high_order_char = yytext[2];
		std::uint8_t high = 0;
		if (high_order_char < 'A') {
			// assume that our regex worked, and we're in the 0-9 range
			high = high_order_char - '0';
		} else {
			// assume that our regex worked, and we're in the A-F range
			high = high_order_char - 'A' + 10;
		}
		high = high * 16;

		char low_order_char = yytext[3];
		std::uint8_t low = 0;
		if (low_order_char < 'A') {
			// assume that our regex worked, and we're in the 0-9 range
			low = low_order_char - '0';
		} else {
			// assume that our regex worked, and we're in the A-F range
			low = low_order_char - 'A' + 10;
		}

		n = high + low;
	} else if (len == 3) {
		char low_order_char = yytext[2];
		if (low_order_char < 'A') {
			// assume that our regex worked, and we're in the 0-9 range
			n = low_order_char - '0';
		} else {
			// assume that our regex worked, and we're in the A-F range
			n = low_order_char - 'A' + 10;
		}
	}

	return yy::newt_parser::make_BYTE_CONSTANT(n, loc);
}

{int} {
	errno = 0;
	long n = strtol (yytext, nullptr, 10);
	if (! (INT_MIN <= n && n <= INT_MAX && errno != ERANGE)) {
		driver.lexer_error(loc, "Integer value is out of range.");
	}
	return yy::newt_parser::make_INT_CONSTANT(n, loc);
}

{double} {
	errno = 0;
	double d = strtod (yytext, nullptr);
	if (! (-DBL_MAX <= d && d <= DBL_MAX && errno != ERANGE)) {
		driver.lexer_error(loc, "Double value is out of range.");
	}
	return yy::newt_parser::make_DOUBLE_CONSTANT(d, loc);
}

{str} {
	uint len = strlen(yytext);
	return yy::newt_parser::make_STRING_CONSTANT(make_shared<const std::string>(yytext + 1, len - 2), loc);
} 

{id} {
	return yy::newt_parser::make_IDENTIFIER(make_shared<const std::string>(yytext), loc);
}

\/\/.* // ignore comments that end a file without a trailing newline 
#.* // ignore comments that end a file without a trailing newline

.               driver.invalid_token(loc, yytext);
<<EOF>> {
	if (yyin) {
		// EOF is called after string parsing too, so we have to check for a valid yyin
		fclose(yyin);
	}
	yy_delete_buffer(YY_CURRENT_BUFFER);

	if (driver.GetInputStack()->empty()) {
		return yy::newt_parser::make_END(loc);
	} else {
		auto input_stack_entry = driver.GetInputStack()->top();
		driver.GetInputStack()->pop();

		yyin = input_stack_entry->GetFileHandle();
		yy_switch_to_buffer(input_stack_entry->GetBufferState());
		loc = input_stack_entry->GetLocation();
	}
}
%%

int Driver::scan_begin(volatile_shared_ptr<string> file_name,
		const bool trace_scanning) {
	yy_flex_debug = trace_scanning;
	if (file_name->empty() || *file_name == "-") {
		yyin = stdin;
	}
	else {
		auto input_file_handle = fopen(file_name->c_str(), "r");
		if (input_file_handle) {
			loc = yy::location(file_name.get(), 1, 1);
			yyin = input_file_handle;
			yy_switch_to_buffer(yy_create_buffer(input_file_handle, YY_BUF_SIZE));
		} else {
			error("Cannot open " + *file_name + ": " + strerror(errno) + "\n");
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

void Driver::scan_end() {
}

int Driver::scan_string_begin(const std::string& string,
		const bool trace_scanning) {
	yy_flex_debug = trace_scanning;
	yy_scan_string(string.c_str());
	return EXIT_SUCCESS;
}

void Driver::scan_string_end() {
}
