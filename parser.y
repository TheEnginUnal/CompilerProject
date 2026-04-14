%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

extern int yylex();
extern int line_num;
extern FILE *yyin;

void yyerror(const char *s);

/* The root of our Abstract Syntax Tree */
struct stmt *parser_result = NULL;
%}

/* --- UNION: The bridge between Flex and Bison --- */
%union {
    struct decl *decl;
    struct stmt *stmt;
    struct expr *expr;
    struct type *type;
    char *name; /* Receives string values from Flex */
}

/* Tokens that carry actual string data from Flex */
%token <name> TOKEN_IDENTIFIER TOKEN_INT_LITERAL TOKEN_STRING_LITERAL TOKEN_CHAR_LITERAL

/* Standard Tokens */
%token TOKEN_ARRAY TOKEN_BOOLEAN TOKEN_CHAR TOKEN_ELSE TOKEN_FALSE TOKEN_FOR
%token TOKEN_FUNCTION TOKEN_IF TOKEN_INTEGER TOKEN_PRINT TOKEN_RETURN
%token TOKEN_STRING TOKEN_TRUE TOKEN_VOID TOKEN_WHILE

%token TOKEN_INCREMENT TOKEN_DECREMENT
%token TOKEN_EQUALITY TOKEN_INEQUALITY TOKEN_LESS_EQUAL TOKEN_GREATER_EQUAL
%token TOKEN_LOGICAL_AND TOKEN_LOGICAL_OR

/* Non-terminal types */
%type <stmt> program statement block_statement statement_list matched_if unmatched_if
%type <decl> declaration
%type <expr> expr
%type <type> type

/* Precedence Rules */
%right '='
%left TOKEN_LOGICAL_OR
%left TOKEN_LOGICAL_AND
%left TOKEN_EQUALITY TOKEN_INEQUALITY
%left '<' '>' TOKEN_LESS_EQUAL TOKEN_GREATER_EQUAL
%left '+' '-'
%left '*' '/' '%'
%right '^'
%right TOKEN_INCREMENT TOKEN_DECREMENT '!'

%nonassoc LOWER_THAN_ELSE
%nonassoc TOKEN_ELSE

%%
/* --- GRAMMAR RULES & AST CONSTRUCTION --- */

program:
      /* empty */ { parser_result = NULL; $$ = NULL; }
    | program statement { 
        if ($1 == NULL) {
            parser_result = $2;
        } else {
            struct stmt *curr = $1;
            while(curr->next != NULL) curr = curr->next;
            curr->next = $2;
            parser_result = $1;
        }
        $$ = parser_result;
    }
    ;

/* A statement covers matched IFs, unmatched IFs, and regular instructions */
statement:
      matched_if { $$ = $1; }
    | unmatched_if { $$ = $1; }
    ;

/* Matched IF-ELSE and basic statements (Base cases) */
matched_if:
      TOKEN_IF '(' expr ')' matched_if TOKEN_ELSE matched_if { 
          $$ = stmt_create(STMT_IF_ELSE, NULL, NULL, $3, NULL, $5, $7, NULL); 
      }
    | expr ';' { $$ = stmt_create(STMT_EXPR, NULL, NULL, $1, NULL, NULL, NULL, NULL); }
    | declaration { $$ = stmt_create(STMT_DECL, $1, NULL, NULL, NULL, NULL, NULL, NULL); }
    | TOKEN_PRINT expr ';' { $$ = stmt_create(STMT_PRINT, NULL, NULL, $2, NULL, NULL, NULL, NULL); }
    | TOKEN_RETURN expr ';' { $$ = stmt_create(STMT_RETURN, NULL, NULL, $2, NULL, NULL, NULL, NULL); }
    | block_statement { $$ = $1; }
    ;

/* Unmatched IF */
unmatched_if:
      TOKEN_IF '(' expr ')' statement %prec LOWER_THAN_ELSE { 
          $$ = stmt_create(STMT_IF_ELSE, NULL, NULL, $3, NULL, $5, NULL, NULL); 
      }
    | TOKEN_IF '(' expr ')' matched_if TOKEN_ELSE unmatched_if { 
          $$ = stmt_create(STMT_IF_ELSE, NULL, NULL, $3, NULL, $5, $7, NULL); 
      }
    ;

declaration:
      TOKEN_IDENTIFIER ':' type ';' { 
          $$ = decl_create($1, $3, NULL, NULL, NULL); 
          free($1);
      }
    | TOKEN_IDENTIFIER ':' type '=' expr ';' { 
          $$ = decl_create($1, $3, $5, NULL, NULL); 
          free($1);
      }
    ;

type:
      TOKEN_INTEGER { $$ = type_create(TYPE_INTEGER, NULL); }
    | TOKEN_BOOLEAN { $$ = type_create(TYPE_BOOLEAN, NULL); }
    | TOKEN_CHAR    { $$ = type_create(TYPE_CHAR, NULL); }
    | TOKEN_STRING  { $$ = type_create(TYPE_STRING, NULL); }
    | TOKEN_VOID    { $$ = type_create(TYPE_VOID, NULL); }
    ;

block_statement:
      '{' statement_list '}' { $$ = stmt_create(STMT_BLOCK, NULL, NULL, NULL, NULL, $2, NULL, NULL); }
    | '{' '}' { $$ = stmt_create(STMT_BLOCK, NULL, NULL, NULL, NULL, NULL, NULL, NULL); }
    ;

statement_list:
      statement { $$ = $1; }
    | statement statement_list { $1->next = $2; $$ = $1; }
    ;

expr:
      TOKEN_INT_LITERAL { $$ = expr_create_integer_literal(atoi($1)); free($1); }
    | TOKEN_STRING_LITERAL { $$ = expr_create_string_literal($1); free($1); }
    | TOKEN_TRUE { $$ = expr_create_boolean_literal(1); }
    | TOKEN_FALSE { $$ = expr_create_boolean_literal(0); }
    | TOKEN_IDENTIFIER { $$ = expr_create_name($1); free($1); }
    | '(' expr ')' { $$ = $2; }
    | expr '+' expr { $$ = expr_create(EXPR_ADD, $1, $3); }
    | expr '-' expr { $$ = expr_create(EXPR_SUB, $1, $3); }
    | expr '*' expr { $$ = expr_create(EXPR_MUL, $1, $3); }
    | expr '/' expr { $$ = expr_create(EXPR_DIV, $1, $3); }
    | expr '%' expr { $$ = expr_create(EXPR_MOD, $1, $3); }
    | expr '^' expr { $$ = expr_create(EXPR_EXP, $1, $3); }
    | expr TOKEN_EQUALITY expr { $$ = expr_create(EXPR_EQUAL, $1, $3); }
    | expr TOKEN_INEQUALITY expr { $$ = expr_create(EXPR_NOT_EQUAL, $1, $3); }
    | expr '<' expr { $$ = expr_create(EXPR_LESS, $1, $3); }
    | expr '>' expr { $$ = expr_create(EXPR_GREATER, $1, $3); }
    | expr TOKEN_LOGICAL_AND expr { $$ = expr_create(EXPR_AND, $1, $3); }
    | expr TOKEN_LOGICAL_OR expr { $$ = expr_create(EXPR_OR, $1, $3); }
    | TOKEN_IDENTIFIER '=' expr { 
          $$ = expr_create(EXPR_ASSIGN, expr_create_name($1), $3); 
          free($1); 
      }
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
    
    printf("=== Starting B-Minor Compiler ===\n");
   if (yyparse() == 0) {
        printf("--- AST BUILT SUCCESSFULLY ---\n\n");
        printf("=== AST PRETTY PRINT OUTPUT ===\n");
        /* Start printing the tree from the root with 0 indentation */
        stmt_print(parser_result, 0); 
        printf("===============================\n");
    } else {
        printf("--- PARSE FAILED ---\n");
    }
    
    return 0;
}