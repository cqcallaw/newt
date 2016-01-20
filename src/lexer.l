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
#include <driver.h>
#include <string.h>
#include "parser.tab.hh"

// The location of the current token.
static yy::location loc;
%}

%option outfile="lexer.cpp"
%option header-file="lexer.h"

%option noyywrap
%option nounput batch debug noinput

id      [_a-zA-Z][a-zA-Z_0-9]*
int     [0-9]+
double  ([0-9]+\.[0-9]+|[0-9]+\.|\.[0-9]+)
str     \"(\\.|[^\"])*\"
blank   [ \t\r]

%{
  #define YY_USER_ACTION  loc.columns (yyleng);
%}

%%

%{
  // Code run each time yylex is called.
  loc.step();
%}

{blank}+   loc.step(); 
[\n]+      loc.lines(yyleng); loc.step();

\/\/.+$         // ignore comments
#.*$            // ignore comments

"bool"          return yy::newt_parser::make_BOOLEAN(loc);
"int"           return yy::newt_parser::make_INT(loc);
"double"        return yy::newt_parser::make_DOUBLE(loc);
"string"        return yy::newt_parser::make_STRING(loc);

"if"            return yy::newt_parser::make_IF(loc);
"for"           return yy::newt_parser::make_FOR(loc);
"else"          return yy::newt_parser::make_ELSE(loc);

"exit"          return yy::newt_parser::make_EXIT(loc);
"print"         return yy::newt_parser::make_PRINT(loc);
"="             return yy::newt_parser::make_EQUALS(loc);
"{"             return yy::newt_parser::make_LBRACE(loc);
"}"             return yy::newt_parser::make_RBRACE(loc);
":"             return yy::newt_parser::make_COLON(loc);
";"             return yy::newt_parser::make_SEMICOLON(loc);
","             return yy::newt_parser::make_COMMA(loc);
"@"             return yy::newt_parser::make_AT(loc);

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
"struct"        return yy::newt_parser::make_STRUCT(loc);
"readonly"      return yy::newt_parser::make_READONLY(loc);
"with"          return yy::newt_parser::make_WITH(loc);
"return"        return yy::newt_parser::make_RETURN(loc);

"true"          return yy::newt_parser::make_TRUE(true, loc);
"false"         return yy::newt_parser::make_FALSE(false, loc);

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
	int len = strlen(yytext);
	return yy::newt_parser::make_STRING_CONSTANT(make_shared<const std::string>(yytext + 1, len - 2), loc);
} 

{id} {
	return yy::newt_parser::make_IDENTIFIER(make_shared<const std::string>(yytext), loc);
}

\/\/.* // ignore comments that end a file without a trailing newline

.               driver.invalid_token(loc, yytext);
<<EOF>>         return yy::newt_parser::make_END(loc);
%%

int Driver::scan_begin(const std::string& file_name,
		const bool trace_scanning) {
	yy_flex_debug = trace_scanning;
	if (file_name.empty() || file_name == "-")
		yyin = stdin;
	else if (!(yyin = fopen(file_name.c_str(), "r"))) {
		error("Cannot open " + file_name + ": " + strerror(errno));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void Driver::scan_end() {
	fclose(yyin);
	yylex_destroy();
}
