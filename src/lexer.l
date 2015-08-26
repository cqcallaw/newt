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

%{

#include <error.h>
#include "parser.tab.h"

void user_init(YYLTYPE* location) {
	location->first_line = 1;
	location->first_column = 1;
	location->last_line = 1;
	location->last_column = 1;
}

void user_action(YYLTYPE* location, char* text) {
	//ref: http://stackoverflow.com/a/22125500/577298
	location->first_line = location->last_line;
	location->first_column = location->last_column;
	for(int i = 0; text[i] != '\0'; i++) {
		if(text[i] == '\n') {
			location->last_line++;
			location->last_column = 1;
		}
		else {
			location->last_column++;
		}
	}
}

#define YY_USER_INIT user_init(yylloc);
#define YY_USER_ACTION user_action(yylloc, yytext);

using namespace std;

Error error_handler; // error reporting object

%}

%option outfile="lexer.c"
%option header-file="lexer.h"

%option noyywrap
%option reentrant
%option bison-bridge
%option bison-locations

%%

[\n]            return T_NEWLINE; 

[ \t\r]         // ignore spaces, tabs and the carriage return
\/\/.*$         // ignore comments

"bool"          return T_BOOLEAN;
"int"           return T_INT;
"double"        return T_DOUBLE;
"string"        return T_STRING;

"if"            return T_IF;
"for"           return T_FOR;
"else"          return T_ELSE;

"exit"          return T_EXIT;
"print"         return T_PRINT;
"true"          return T_TRUE;
"false"         return T_FALSE;

"="             return T_ASSIGN;
"{"             return T_LBRACE;
"}"             return T_RBRACE;
";"             return T_SEMIC;
","             return T_COMMA;

"("             return T_LPAREN;
")"             return T_RPAREN;
"["             return T_LBRACKET;
"]"             return T_RBRACKET;
"."             return T_PERIOD;
"+="            return T_PLUS_ASSIGN;
"-="            return T_MINUS_ASSIGN;

"*"             return T_ASTERISK;
"/"             return T_DIVIDE;
"+"             return T_PLUS;
"-"             return T_MINUS;
"%"             return T_MOD;
"<"             return T_LESS;
">"             return T_GREATER;
"<="            return T_LESS_EQUAL;
">="            return T_GREATER_EQUAL;
"=="            return T_EQUAL;
"!="            return T_NOT_EQUAL;
"!"             return T_NOT;
"&&"            return T_AND;
"||"            return T_OR;

[0-9]+                              { yylval->union_int = atoi(yytext); return T_INT_CONSTANT; }
([0-9]+\.[0-9]+|[0-9]+\.|\.[0-9]+)  { yylval->union_double = atof(yytext); return T_DOUBLE_CONSTANT; }
\"(\\.|[^\"])*\"                    { string* as_string = new string(yytext); yylval->union_string = new string(as_string->substr(1, as_string->length() - 2)); delete (as_string); return T_STRING_CONSTANT; } //regex taken from http://stackoverflow.com/a/2039820/577298 and http://stackoverflow.com/a/9260547/577298

[_A-Za-z][_A-Za-z0-9]*              { yylval->union_string = new string(yytext); return T_ID; }

.                                   { error_handler.lex_error(yylineno, yytext); yylval->union_string = new string(yytext); return T_ERROR; }
