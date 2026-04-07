%{
#include <stdio.h>
#include <stdlib.h>

extern int yylex();
extern int line_num;
extern FILE *yyin;

void yyerror(const char *s);
%}

/* --- TOKEN DEFINITIONS --- */
%token TOKEN_ARRAY TOKEN_BOOLEAN TOKEN_CHAR TOKEN_ELSE TOKEN_FALSE TOKEN_FOR
%token TOKEN_FUNCTION TOKEN_IF TOKEN_INTEGER TOKEN_PRINT TOKEN_RETURN
%token TOKEN_STRING TOKEN_TRUE TOKEN_VOID TOKEN_WHILE

%token TOKEN_IDENTIFIER TOKEN_INT_LITERAL TOKEN_STRING_LITERAL TOKEN_CHAR_LITERAL

%token TOKEN_INCREMENT TOKEN_DECREMENT
%token TOKEN_EQUALITY TOKEN_INEQUALITY TOKEN_LESS_EQUAL TOKEN_GREATER_EQUAL
%token TOKEN_LOGICAL_AND TOKEN_LOGICAL_OR

/* --- OPERATOR PRECEDENCE & ASSOCIATIVITY --- */
/* Lowest to highest precedence. 
   This solves the ambiguous grammar problem for math and logic!
*/
%right '='
%left TOKEN_LOGICAL_OR
%left TOKEN_LOGICAL_AND
%left TOKEN_EQUALITY TOKEN_INEQUALITY
%left '<' '>' TOKEN_LESS_EQUAL TOKEN_GREATER_EQUAL
%left '+' '-'
%left '*' '/' '%'
%right '^'
%right TOKEN_INCREMENT TOKEN_DECREMENT '!'

/* DANGLING-ELSE SOLUTION:
   We declare a dummy token with lower precedence than ELSE to force
   the parser to shift the ELSE to the closest IF.
*/
%nonassoc LOWER_THAN_ELSE
%nonassoc TOKEN_ELSE

%%
/* --- GRAMMAR RULES --- */

program:
      /* empty */
    | program declaration
    | program statement
    ;

/* Variables and Function Declarations */
declaration:
      TOKEN_IDENTIFIER ':' type ';' { printf("PARSED: Variable declaration\n"); }
    | TOKEN_IDENTIFIER ':' type '=' expr ';' { printf("PARSED: Variable declaration with initialization\n"); }
    ;

/* Base Types */
type:
      TOKEN_INTEGER
    | TOKEN_BOOLEAN
    | TOKEN_CHAR
    | TOKEN_STRING
    | TOKEN_VOID
    ;

/* Statements (Instructions) */
statement:
      expr ';' { printf("PARSED: Expression statement\n"); }
    | TOKEN_PRINT expr ';' { printf("PARSED: Print statement\n"); }
    | TOKEN_RETURN expr ';' { printf("PARSED: Return statement\n"); }
    | block_statement
    | matched_if
    | unmatched_if
    ;

/* Block of statements inside { } */
block_statement:
      '{' statement_list '}' { printf("PARSED: Block statement\n"); }
    | '{' '}' { printf("PARSED: Empty block\n"); }
    ;

statement_list:
      statement
    | statement_list statement
    ;

/* IF-ELSE RESOLUTION (Solving Dangling-Else)
   We separate "matched" (has both if and else) from "unmatched" (only has if).
*/
matched_if:
      TOKEN_IF '(' expr ')' matched_if TOKEN_ELSE matched_if { printf("PARSED: Matched IF-ELSE\n"); }
    | block_statement
    | expr ';'
    ;

unmatched_if:
      TOKEN_IF '(' expr ')' statement %prec LOWER_THAN_ELSE { printf("PARSED: IF without ELSE\n"); }
    | TOKEN_IF '(' expr ')' matched_if TOKEN_ELSE unmatched_if { printf("PARSED: Complex IF-ELSE\n"); }
    ;

/* Expressions (Math, Logic, Function Calls) */
expr:
      TOKEN_INT_LITERAL
    | TOKEN_CHAR_LITERAL
    | TOKEN_STRING_LITERAL
    | TOKEN_TRUE
    | TOKEN_FALSE
    | TOKEN_IDENTIFIER
    | '(' expr ')'
    | expr '+' expr
    | expr '-' expr
    | expr '*' expr
    | expr '/' expr
    | expr '%' expr
    | expr '^' expr
    | expr TOKEN_EQUALITY expr
    | expr TOKEN_INEQUALITY expr
    | expr '<' expr
    | expr '>' expr
    | expr TOKEN_LESS_EQUAL expr
    | expr TOKEN_GREATER_EQUAL expr
    | expr TOKEN_LOGICAL_AND expr
    | expr TOKEN_LOGICAL_OR expr
    | TOKEN_IDENTIFIER '=' expr
    | TOKEN_INCREMENT TOKEN_IDENTIFIER
    | TOKEN_DECREMENT TOKEN_IDENTIFIER
    | TOKEN_IDENTIFIER TOKEN_INCREMENT
    | TOKEN_IDENTIFIER TOKEN_DECREMENT
    | '!' expr
    | '-' expr %prec '*' /* Unary minus precedence trick */
    ;

%%
/* --- USER CODE --- */

void yyerror(const char *s) {
    fprintf(stderr, "Syntax Error at line %d: %s\n", line_num, s);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            fprintf(stderr, "Error: Could not open file %s\n", argv[1]);
            return 1;
        }
        yyin = file;
    }
    
    printf("=== Starting B-Minor Parser ===\n");
    if (yyparse() == 0) {
        printf("--- PARSE SUCCESSFUL ---\n");
    } else {
        printf("--- PARSE FAILED ---\n");
    }
    
    return 0;
}