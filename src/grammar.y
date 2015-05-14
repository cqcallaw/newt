%{

extern int yylex();               // this lexer function returns next token
extern int yyerror(const char *); // used to print errors
extern int line_count;            // current line in the input; from lexer

#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>
#include <algorithm>

#include <error.h>
#include <parser.h>
#include <utils.h>
#include <constant_expression.h>
#include <comparison_expression.h>
#include <arithmetic_expression.h>
#include <logic_expression.h>
#include <unary_expression.h>
#include <binary_expression.h>
#include <string_concatenation_expression.h>
#include <variable_expression.h>
#include <print_statement.h>
#include <assignment_statement.h>
#include <exit_statement.h>
#include <if_statement.h>
#include <for_statement.h>
#include <statement_block.h>
#include <defaults.h>

%}

%union {
 bool           union_boolean;
 int            union_int;
 double         union_double;
 std::string    *union_string;
 Type       union_type;
 const Expression*    union_expression;
 const Variable*    union_variable;
 OperatorType  union_operator_type;
 const Parameter* union_parameter_type;
 const ParameterList* union_parameter_list_type;
 const Symbol* union_symbol_type;
 const Statement* union_statement_type;
 const AssignmentStatement* union_assignment_statement_type;
 const StatementList* union_statement_list_type;
 const StatementBlock* union_statement_block_type;
}

%error-verbose

%token T_BOOLEAN               "bool"
%token T_INT                   "int"
%token T_DOUBLE                "double"
%token T_STRING                "string"

%token <union_boolean> T_TRUE  "boolean true" 
%token <union_boolean> T_FALSE "boolean false" 

%token T_IF                    "if"
%token T_FOR                   "for"
%token T_ELSE                  "else"
%token <union_int> T_PRINT     "print" // value is line number
%token <union_int> T_EXIT      "exit" // value is line number

%token T_LPAREN              "("
%token T_RPAREN              ")"
%token T_LBRACE              "{"
%token T_RBRACE              "}"
%token T_LBRACKET            "["
%token T_RBRACKET            "]"
%token T_SEMIC               ";"
%token T_COMMA               ","
%token T_PERIOD              "."

%token T_ASSIGN              "="
%token T_PLUS_ASSIGN         "+="
%token T_MINUS_ASSIGN        "-="

%token T_ASTERISK            "*"
%token T_DIVIDE              "/"
%token T_MOD                 "%"
%token T_PLUS                "+"
%token T_MINUS               "-"
%token T_SIN                 "sin"
%token T_COS                 "cos"
%token T_TAN                 "tan"
%token T_ASIN                "asin"
%token T_ACOS                "acos"
%token T_ATAN                "atan"
%token T_SQRT                "sqrt"
%token T_FLOOR               "floor"
%token T_ABS                 "abs"
%token T_RANDOM              "random"

%token T_LESS                "<"
%token T_GREATER             ">"
%token T_LESS_EQUAL          "<="
%token T_GREATER_EQUAL       ">="
%token T_EQUAL               "=="
%token T_NOT_EQUAL           "!="

%token T_AND                 "&&"
%token T_OR                  "||"
%token T_NOT                 "!"

%token <union_string> T_ID               "identifier"
%token <union_int> T_INT_CONSTANT        "int constant"
%token <union_double > T_DOUBLE_CONSTANT "double constant"
%token <union_string> T_STRING_CONSTANT  "string constant"

%token T_ERROR               "error" // error token

%left T_OR
%left T_AND
%left T_EQUAL T_NOT_EQUAL
%left T_LESS T_LESS_EQUAL T_GREATER T_GREATER_EQUAL
%left T_PLUS T_MINUS
%left T_ASTERISK T_DIVIDE T_MOD

%right T_ASSIGN
%right T_PLUS_ASSIGN
%right T_MINUS_ASSIGN

%nonassoc UNARY_OPS

%precedence IF_NO_ELSE
%precedence T_ELSE

%type <union_type> simple_type
%type <union_expression> expression
%type <union_expression> primary_expression
%type <union_expression> optional_initializer
%type <union_expression> variable_declaration
%type <union_variable> variable_reference
%type <union_operator_type> math_operator
%type <union_statement_type> statement
%type <union_statement_list_type> statement_list
%type <union_statement_block_type> if_block
%type <union_statement_block_type> statement_block
%type <union_statement_type> if_statement
%type <union_assignment_statement_type> assign_statement
%type <union_statement_type> print_statement
%type <union_statement_type> for_statement
%type <union_statement_type> exit_statement

%% // begin rules

//---------------------------------------------------------------------
program:
	declaration_list main_statement_block
	{
	}
	;

//---------------------------------------------------------------------
declaration_list:
	declaration_list declaration
	| empty
	;

//---------------------------------------------------------------------
declaration:
	variable_declaration T_SEMIC
	;

//---------------------------------------------------------------------
variable_declaration:
	simple_type T_ID optional_initializer
	{
		Symbol* symbol;
		switch($1)
		{
			case BOOLEAN:
			{
				bool *value;

				if($3 != NULL && $3->GetType() != NONE)
				{
					const void* evaluation = $3->Evaluate();
					if($3->GetType() == BOOLEAN)
					{
						if(evaluation != NULL)
						{
							value = (bool*)evaluation;
						}
						else
						{
							break;
						}
					}
					else
					{
						Error::error(Error::INVALID_TYPE_FOR_INITIAL_VALUE, *$2);
					}
				}
				else
				{
					value = new bool(false);
				}

				symbol = new Symbol($2, value);
				InsertResult result = Symbol_table::instance()->InsertSymbol(symbol);
				if (result == SYMBOL_EXISTS)
				{
					Error::error(Error::PREVIOUSLY_DECLARED_VARIABLE, *$2);
				}
				break;
			}
			case INT:
			{
				int *value;

				if($3 != NULL && $3->GetType() != NONE)
				{
					const void* evaluation = $3->Evaluate();
					if($3->GetType() == BOOLEAN)
					{
						if(evaluation != NULL)
						{
							value = new int(*((bool*)evaluation));
						}
						else
						{
							break;
						}
					}
					else if($3->GetType() == INT)
					{
						if(evaluation != NULL)
						{
							value = (int*)(evaluation);
						}
						else
						{
							break;
						}
					}
					else
					{
						Error::error(Error::INVALID_TYPE_FOR_INITIAL_VALUE, *$2);
					}
				}
				else
				{
					value = new int(0);
				}

				symbol = new Symbol($2, value);
				InsertResult result = Symbol_table::instance()->InsertSymbol(symbol);
				if (result == SYMBOL_EXISTS)
				{
					Error::error(Error::PREVIOUSLY_DECLARED_VARIABLE, *$2);
				}
				break;
			}
			case DOUBLE:
			{
				double* value;

				if($3 != NULL && $3->GetType() != NONE)
				{
					const void* evaluation = $3->Evaluate();
					if ($3->GetType() == BOOLEAN)
					{
						if(evaluation != NULL)
						{
							value = new double(*((bool*)(evaluation)));
						}
						else
						{
							break;
						}
					}
					else if ($3->GetType() == INT)
					{
						if(evaluation != NULL)
						{
							value = new double(*((int*)(evaluation)));
						}
						else
						{
							break;
						}
					}
					else if($3->GetType() == DOUBLE)
					{
						if(evaluation != NULL)
						{
							value = (double*)(evaluation);
						}
						else
						{
							break;
						}
					}
					else
					{
						Error::error(Error::INVALID_TYPE_FOR_INITIAL_VALUE, *$2);
					}
				}
				else
				{
					value = new double(0.0);
				}

				symbol = new Symbol($2, value);
				InsertResult result = Symbol_table::instance()->InsertSymbol(symbol);
				if (result == SYMBOL_EXISTS)
				{
					Error::error(Error::PREVIOUSLY_DECLARED_VARIABLE, *$2);
				}
				break;
			}
			case STRING:
			{
				string* value;

				if($3 != NULL && $3->GetType() != NONE)
				{
					Type initializer_type = $3->GetType();
					const void* evaluation = $3->Evaluate();
					if(evaluation == NULL)
					{
						break;
					}

					ostringstream buffer;
					switch (initializer_type) {
						case STRING: {
							value = (string*)(evaluation);
							break;
						}
						case BOOLEAN: {
							value = AsString((bool*)(evaluation));
							break;
						}
						case INT: {
							value = AsString((int*)(evaluation));
							break;
						}
						case DOUBLE: {
							value = AsString((double*)(evaluation));
							break;
						}
						default:
							Error::error(Error::INVALID_TYPE_FOR_INITIAL_VALUE, *$2);
							value = new string("");
					}
				}
				else
				{
					value = new string("");
				}

				symbol = new Symbol($2, value);
				InsertResult result = Symbol_table::instance()->InsertSymbol(symbol);
				if (result == SYMBOL_EXISTS)
				{
					Error::error(Error::PREVIOUSLY_DECLARED_VARIABLE, *$2);
				}
				break;
			}
			default:
				break;
		}
	}
	| simple_type  T_ID  T_LBRACKET expression T_RBRACKET
	{
		const Symbol* existing_symbol = Symbol_table::instance()->GetSymbol($2);

		if (existing_symbol != Symbol::DefaultSymbol)
		{
			Error::error(Error::PREVIOUSLY_DECLARED_VARIABLE, *$2);
		}

		if($4->GetType() != INT)
		{
			Error::error(Error::INVALID_ARRAY_SIZE, *$2, *($4->ToString()));
		}
		else
		{
			const void* evaluation = $4->Evaluate();
			if (evaluation != NULL)
			{
				int array_size = *((int*)(evaluation));

				if (array_size <= 0)
				{
					ostringstream convert;
					convert << array_size;
					Error::error(Error::INVALID_ARRAY_SIZE, *$2, convert.str());
					$$ = (Expression*)DefaultExpression;
				}
				else
				{
					Symbol* symbol;

					switch($1)
					{
						case INT:
						{
							int* array = new int[array_size]();

							for(int i = 0; i < array_size; i++)
							{
								array[i] = 0;
							}

							symbol = (Symbol*)new ArraySymbol($2, array, array_size);
							Symbol_table::instance()->InsertSymbol(symbol);

							break;
						}
						case DOUBLE:
						{
							double* array = new double[array_size]();

							for(int i = 0; i < array_size; i++)
							{
								array[i] = 0.0;
							}

							symbol = (Symbol*)new ArraySymbol($2, array, array_size);
							Symbol_table::instance()->InsertSymbol(symbol);

							break;
						}
						case STRING:
						{
							const string** array = (const string**)new string*[array_size]();

							for(int i = 0; i < array_size; i++)
							{
								array[i] = new string("");
							}

							symbol = (Symbol*)new ArraySymbol($2, array, array_size);
							Symbol_table::instance()->InsertSymbol(symbol);

							break;
						}
						default:
							break;
					}
				}
			}
		}
	}
	;

//---------------------------------------------------------------------
simple_type:
	T_BOOLEAN
	{
		$$ = BOOLEAN;
	}
	|
	T_INT
	{
		$$ = INT;
	}
	| T_DOUBLE
	{
		$$ = DOUBLE;
	}
	| T_STRING
	{
		$$ = STRING;
	}
	;

//---------------------------------------------------------------------
optional_initializer:
	T_ASSIGN expression
	{
		$$ = $2;
	}
	| empty
	{
		$$ = NULL;
	}
	;

//---------------------------------------------------------------------
if_block:
	statement
	{
		$$ = new StatementBlock(new StatementList($1, StatementList::Terminator));
	}
	| statement_block
	{
		$$ = $1;
	}
	;
	
//---------------------------------------------------------------------
main_statement_block:
	statement_block
	|
	empty
	;

//---------------------------------------------------------------------
statement_block:
	T_LBRACE statement_list T_RBRACE
	{
		$$ = new StatementBlock($2->Reverse(true)); //statement list comes in reverse order
	}
	;

//---------------------------------------------------------------------
statement_list:
	statement_list statement
	{
		$$ = new StatementList($2, $1);
	}
	| empty
	{
		$$ = StatementList::Terminator;
	}
	;

//---------------------------------------------------------------------
statement:
	if_statement
	{
		$$ = $1;
	}
	| for_statement
	{
		$$ = $1;
	}
	| assign_statement T_SEMIC
	{
		$$ = $1;
	}
	| print_statement T_SEMIC
	{
		$$ = $1;
	}
	| exit_statement T_SEMIC
	{
		$$ = $1;
	}
	;

//---------------------------------------------------------------------
if_statement:
	T_IF T_LPAREN expression T_RPAREN if_block %prec IF_NO_ELSE
	{
		if (!($3->GetType() & (BOOLEAN | INT))) {
			Error::error(Error::INVALID_TYPE_FOR_IF_STMT_EXPRESSION);
			$$ = DefaultIfStatement;
		} else { 
			$$ = new IfStatement($3, $5);
		}
	}
	| T_IF T_LPAREN expression T_RPAREN if_block T_ELSE if_block
	{
		if (!($3->GetType() & (BOOLEAN | INT))) {
			Error::error(Error::INVALID_TYPE_FOR_IF_STMT_EXPRESSION);
			$$ = DefaultIfStatement;
		} else { 
			$$ = new IfStatement($3, $5, $7);
		}
	}
	;

//---------------------------------------------------------------------
for_statement:
	T_FOR T_LPAREN assign_statement T_SEMIC expression T_SEMIC assign_statement T_RPAREN statement_block
	{
		if (!($5->GetType() & (BOOLEAN | INT))) {
			Error::error(Error::INVALID_TYPE_FOR_FOR_STMT_EXPRESSION);
			$$ = DefaultForStatement;
		} else { 
			$$ = new ForStatement($3, $5, $7, $9);
		}
	}
	;

//---------------------------------------------------------------------
print_statement:
	T_PRINT T_LPAREN expression T_RPAREN
	{
		Type expression_type = $3->GetType();
		if (expression_type > STRING) {
			Error::error(Error::INVALID_TYPE_FOR_PRINT_STMT_EXPRESSION, type_to_string(expression_type));
			$$ = DefaultPrintStatement;
		} else {
			$$ = new PrintStatement($1, $3);
		}
	}
	;

//---------------------------------------------------------------------
exit_statement:
	T_EXIT T_LPAREN expression T_RPAREN
	{
		Type expression_type = $3->GetType();
		if (expression_type != INT) {
			Error::error(Error::EXIT_STATUS_MUST_BE_AN_INTEGER, type_to_string(expression_type));
			$$ = DefaultExitStatement;
		} else {
			$$ = new ExitStatement($1, $3);
		}
	}
	;

//---------------------------------------------------------------------
assign_statement:
	variable_reference T_ASSIGN expression
	{
		Type variable_type = $1->GetType();
		Type expression_type = $3->GetType();
		if (variable_type == NONE || expression_type == NONE) {
			$$ = DefaultAssignmentStatement;
		} else if (!(variable_type & (BOOLEAN | INT | DOUBLE | STRING))) {
			Error::error(Error::INVALID_LHS_OF_ASSIGNMENT, *($1->GetName()), type_to_string(variable_type));
			$$ = DefaultAssignmentStatement;
		} else if (variable_type <= STRING && expression_type > variable_type) {
			Error::error(Error::ASSIGNMENT_TYPE_ERROR, type_to_string(variable_type), type_to_string(expression_type));
			$$ = DefaultAssignmentStatement;
		} else {
			$$ = new AssignmentStatement($1, AssignmentStatement::AssignmentType::ASSIGN, $3);
		} 
	}
	| variable_reference T_PLUS_ASSIGN expression
	{
		string variable_name = *($1->GetName());
		Type variable_type = $1->GetType();
		Type expression_type = $3->GetType();
		if (variable_type == NONE || expression_type == NONE) {
			$$ = DefaultAssignmentStatement;
		} else if (variable_type <= STRING && expression_type > variable_type) {
			Error::error(Error::PLUS_ASSIGNMENT_TYPE_ERROR, type_to_string(variable_type), type_to_string(expression_type));
			$$ = DefaultAssignmentStatement;
		} else if (!(variable_type & (INT | DOUBLE | STRING))) {
			Error::error(Error::INVALID_LHS_OF_PLUS_ASSIGNMENT, variable_name, type_to_string(variable_type));
			$$ = DefaultAssignmentStatement;
		} else {
			$$ = new AssignmentStatement($1, AssignmentStatement::AssignmentType::PLUS_ASSIGN, $3);
		} 
	}
	| variable_reference T_MINUS_ASSIGN expression
	{
		string variable_name = *($1->GetName());
		Type variable_type = $1->GetType();
		Type expression_type = $3->GetType();
		if (variable_type == NONE || expression_type == NONE) {
			$$ = DefaultAssignmentStatement;
		} else if (variable_type <= STRING && expression_type > variable_type) {
			Error::error(Error::MINUS_ASSIGNMENT_TYPE_ERROR, type_to_string(variable_type), type_to_string(expression_type));
			$$ = DefaultAssignmentStatement;
		} else if (!(variable_type & (INT | DOUBLE))) {
			Error::error(Error::INVALID_LHS_OF_MINUS_ASSIGNMENT, variable_name, type_to_string(variable_type));
			$$ = DefaultAssignmentStatement;
		} else {
			$$ = new AssignmentStatement($1, AssignmentStatement::AssignmentType::MINUS_ASSIGN, $3);
		} 
	}
	;

//---------------------------------------------------------------------
variable_reference:
	T_ID
	{
		const Symbol* symbol = Symbol_table::instance()->GetSymbol($1);

		if(symbol == NULL || symbol == Symbol::DefaultSymbol)
		{
			Error::error(Error::UNDECLARED_VARIABLE, *$1);
			$$ = Variable::DefaultVariable;
		}
		else
		{
			if (symbol->GetType() & (INT_ARRAY | DOUBLE_ARRAY | STRING_ARRAY))
			{
				Error::error(Error::UNDECLARED_VARIABLE, *$1);
			}

			$$ = new Variable($1);
		}
	}
	| T_ID T_LBRACKET expression T_RBRACKET
	{
		const Symbol* symbol = Symbol_table::instance()->GetSymbol($1);

		if(symbol == Symbol::DefaultSymbol)
		{
			Error::error(Error::UNDECLARED_VARIABLE, *$1);
			$$ = Variable::DefaultVariable;
		}
		else
		{
			if ($3->GetType() != INT)
			{
				switch($3->GetType())
				{
					case BOOLEAN:
						Error::error(Error::ARRAY_INDEX_MUST_BE_AN_INTEGER, *$1, "A boolean expression");
						break;
					case DOUBLE:
						Error::error(Error::ARRAY_INDEX_MUST_BE_AN_INTEGER, *$1, "A double expression");
						break;
					case STRING:
						Error::error(Error::ARRAY_INDEX_MUST_BE_AN_INTEGER, *$1, "A string expression");
						break;
				}
				$$ = Variable::DefaultVariable;
			}
			else if (symbol->GetType() & (INT_ARRAY | DOUBLE_ARRAY | STRING_ARRAY))
			{
				$$ = new ArrayVariable($1, $3);
			}
			else
			{
				Error::error(Error::VARIABLE_NOT_AN_ARRAY, *$1);
				$$ = Variable::DefaultVariable;
			}
		}
	}
	| T_ID T_PERIOD T_ID
	{
	}
	| T_ID T_LBRACKET expression T_RBRACKET T_PERIOD T_ID
	{
	}
	;

//---------------------------------------------------------------------
expression:
	primary_expression
	{
		$$ = $1;
	}
	| expression T_OR expression
	{
		if ($1->GetType() != NONE && !($1->GetType() & (BOOLEAN | INT | DOUBLE)))
		{
			Error::error(Error::INVALID_LEFT_OPERAND_TYPE, operator_to_string(OR));
			$$ = DefaultExpression;
		}
		else if ($3->GetType() != NONE && !($3->GetType() & (BOOLEAN | INT | DOUBLE)))
		{
			Error::error(Error::INVALID_RIGHT_OPERAND_TYPE, operator_to_string(OR));
			$$ = DefaultExpression;
		}
		else
		{
			$$ = new LogicExpression(OR, $1, $3);
		}
	}
	| expression T_AND expression
	{
		if ($1->GetType() != NONE && !($1->GetType() & (BOOLEAN | INT | DOUBLE)))
		{
			Error::error(Error::INVALID_LEFT_OPERAND_TYPE, operator_to_string(AND));
			$$ = DefaultExpression;
		}
		else if ($3->GetType() != NONE && !($3->GetType() & (BOOLEAN | INT | DOUBLE)))
		{
			Error::error(Error::INVALID_RIGHT_OPERAND_TYPE, operator_to_string(AND));
			$$ = DefaultExpression;
		}
		else
		{
			$$ = new LogicExpression(AND, $1, $3);
		}
	}
	| expression T_LESS_EQUAL expression
	{
		$$ = new ComparisonExpression(LESS_THAN_EQUAL, $1, $3);
	}
	| expression T_GREATER_EQUAL  expression
	{
		$$ = new ComparisonExpression(GREATER_THAN_EQUAL, $1, $3);
	}
	| expression T_LESS expression 
	{
		$$ = new ComparisonExpression(LESS_THAN, $1, $3);
	}
	| expression T_GREATER  expression
	{
		$$ = new ComparisonExpression(GREATER_THAN, $1, $3);
	}
	| expression T_EQUAL expression
	{
		$$ = new ComparisonExpression(EQUAL, $1, $3);
	}
	| expression T_NOT_EQUAL expression
	{
		$$ = new ComparisonExpression(NOT_EQUAL, $1, $3);
	}
	| expression T_PLUS expression 
	{
		if ($1->GetType() == STRING || $3->GetType() == STRING)
		{
			$$ = new StringConcatenationExpression($1, $3);
		}
		else
		{
			$$ = new ArithmeticExpression(PLUS, $1, $3);
		}
	}
	| expression T_MINUS expression
	{
		if ($1->GetType() != NONE && !($1->GetType() & (INT | DOUBLE)))
		{
			Error::error(Error::INVALID_LEFT_OPERAND_TYPE, operator_to_string(MINUS));
			$$ = DefaultExpression;
		}
		else if ($3->GetType() != NONE && !($3->GetType() & (INT | DOUBLE)))
		{
			Error::error(Error::INVALID_RIGHT_OPERAND_TYPE, operator_to_string(MINUS));
			$$ = DefaultExpression;
		}
		else
		{
			$$ = new ArithmeticExpression(MINUS, $1, $3);
		}
	}
	| expression T_ASTERISK expression
	{
		if ($1->GetType() != NONE && !($1->GetType() & (INT | DOUBLE)))
		{
			Error::error(Error::INVALID_LEFT_OPERAND_TYPE, operator_to_string(MULTIPLY));
			$$ = DefaultExpression;
		}
		else if ($3->GetType() != NONE && !($3->GetType() & (INT | DOUBLE)))
		{
			Error::error(Error::INVALID_RIGHT_OPERAND_TYPE, operator_to_string(MULTIPLY));
			$$ = DefaultExpression;
		}
		else
		{
			$$ = new ArithmeticExpression(MULTIPLY, $1, $3);
		}
	}
	| expression T_DIVIDE expression
	{
		if ($1->GetType() != NONE && !($1->GetType() & (INT | DOUBLE)))
		{
			Error::error(Error::INVALID_LEFT_OPERAND_TYPE, operator_to_string(DIVIDE));
			$$ = DefaultExpression;
		}
		else if ($3->GetType() != NONE && !($3->GetType() & (INT | DOUBLE)))
		{
			Error::error(Error::INVALID_RIGHT_OPERAND_TYPE, operator_to_string(DIVIDE));
			$$ = DefaultExpression;
		}
		else if ($3->GetType() & (BOOLEAN | INT | DOUBLE) && EndsWith(typeid(*$3).name(), "ConstantExpression"))
		{
			//handle divide by zero
			//ref: http://stackoverflow.com/a/11310937/577298
			if ($3->GetType() == BOOLEAN && *((bool*)$3->Evaluate()) == false)
			{
				Error::error(Error::DIVIDE_BY_ZERO_AT_PARSE_TIME);
				$$ = new ConstantExpression(0);
			}
			else if ($3->GetType() == INT && *((int*)$3->Evaluate()) == 0)
			{
				Error::error(Error::DIVIDE_BY_ZERO_AT_PARSE_TIME);
				$$ = new ConstantExpression(0);
			}
			else if ($3->GetType() == DOUBLE && *((double*)$3->Evaluate()) == 0.0)
			{
				Error::error(Error::DIVIDE_BY_ZERO_AT_PARSE_TIME);
				$$ = new ConstantExpression(0);
			}
			else
			{
				$$ = new ArithmeticExpression(DIVIDE, $1, $3);
			}
		}
		else
		{
			$$ = new ArithmeticExpression(DIVIDE, $1, $3);
		}
	}
	| expression T_MOD expression
	{
		if ($1->GetType() != NONE && !($1->GetType() & INT))
		{
			Error::error(Error::INVALID_LEFT_OPERAND_TYPE, operator_to_string(MOD));
			$$ = DefaultExpression;
		}
		else if ($3->GetType() != NONE && !($3->GetType() & INT))
		{
			Error::error(Error::INVALID_RIGHT_OPERAND_TYPE, operator_to_string(MOD));
			$$ = DefaultExpression;
		}
		else if ($3->GetType() & (BOOLEAN | INT | DOUBLE) && EndsWith(typeid(*$3).name(), "ConstantExpression"))
		{
			//handle divide by zero
			//ref: http://stackoverflow.com/a/11310937/577298
			if ($3->GetType() == BOOLEAN && *((bool*)$3->Evaluate()) == false)
			{
				Error::error(Error::MOD_BY_ZERO_AT_PARSE_TIME);
				$$ = new ConstantExpression(0);
			}
			else if ($3->GetType() == INT && *((int*)$3->Evaluate()) == 0)
			{
				Error::error(Error::MOD_BY_ZERO_AT_PARSE_TIME);
				$$ = new ConstantExpression(0);
			}
			else if ($3->GetType() == DOUBLE && *((double*)$3->Evaluate()) == 0.0)
			{
				Error::error(Error::MOD_BY_ZERO_AT_PARSE_TIME);
				$$ = new ConstantExpression(0);
			}
			else
			{
				$$ = new ArithmeticExpression(MOD, $1, $3);
			}
		}
		else
		{
			$$ = new ArithmeticExpression(MOD, $1, $3);
		}
	}
	| T_MINUS expression %prec UNARY_OPS
	{
		if($2->GetType() & (INT | DOUBLE))
		{
			$$ = new UnaryExpression(UNARY_MINUS, $2);
		}
		else
		{
			Error::error(Error::UNDEFINED_ERROR);
			$$ = DefaultExpression;
		}
	}
	| T_NOT expression %prec UNARY_OPS
	{
		if ($2->GetType() != NONE && !($2->GetType() & (BOOLEAN | INT | DOUBLE)))
		{
			Error::error(Error::INVALID_RIGHT_OPERAND_TYPE, operator_to_string(NOT));
			$$ = DefaultExpression;
		}
		else
		{
			$$ = new UnaryExpression(NOT, $2);
		}
	}
	| math_operator T_LPAREN expression T_RPAREN
	{
		if($3->GetType() & (INT | DOUBLE))
		{
			$$ = new UnaryExpression($1, $3);
		}
		else
		{
			Error::error(Error::INVALID_RIGHT_OPERAND_TYPE, operator_to_string($1));
			$$ = DefaultExpression;
		}
	}
	;

//---------------------------------------------------------------------
primary_expression:
	T_LPAREN expression T_RPAREN
	{
		$$ = $2;
	}
	| variable_reference
	{
		$$ = new VariableExpression($1);
	}
	| T_INT_CONSTANT
	{
		$$ = new ConstantExpression($1);
	}
	| T_TRUE
	{
		$$ = new ConstantExpression(true);
	}
	| T_FALSE
	{
		$$ = new ConstantExpression(false);
	}
	| T_DOUBLE_CONSTANT
	{
		$$ = new ConstantExpression($1);
	}
	| T_STRING_CONSTANT
	{
		$$ = new ConstantExpression($1);
	}
	;

//---------------------------------------------------------------------
math_operator:
	T_SIN
	{
		$$ = SIN;
	}
	| T_COS
	{
		$$ = COS;
	}
	| T_TAN
	{
		$$ = TAN;
	}
	| T_ASIN
	{
		$$ = ASIN;
	}
	| T_ACOS
	{
		$$ = ACOS;
	}
	| T_ATAN
	{
		$$ = ATAN;
	}
	| T_SQRT
	{
		$$ = SQRT;
	}
	| T_ABS
	{
		$$ = ABS;
	}
	| T_FLOOR
	{
		$$ = FLOOR;
	}
	| T_RANDOM
	{
		$$ = RANDOM;
	}
	;

//---------------------------------------------------------------------
empty:
	;
