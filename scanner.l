%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.tab.h"

char string_buffer[257]; 
int string_length = 0;
int line_num = 1;
%}

%option noyywrap
%option yylineno
%option nounput
%option noinput

DIGIT      [0-9]
LETTER     [a-zA-Z]
IDENTIFIER [a-zA-Z_][a-zA-Z0-9_]*

%x COMMENT_STATE


%%

[ \t\r]+    { /* Do nothing */ }
\n          { line_num++; }

"array"     { return TOKEN_ARRAY; }
"boolean"   { return TOKEN_BOOLEAN; }
"char"      { return TOKEN_CHAR; }
"else"      { return TOKEN_ELSE; }
"false"     { return TOKEN_FALSE; }
"for"       { return TOKEN_FOR; }
"function"  { return TOKEN_FUNCTION; }
"if"        { return TOKEN_IF; }
"integer"   { return TOKEN_INTEGER; }
"print"     { return TOKEN_PRINT; }
"return"    { return TOKEN_RETURN; }
"string"    { return TOKEN_STRING; }
"true"      { return TOKEN_TRUE; }
"void"      { return TOKEN_VOID; }
"while"     { return TOKEN_WHILE; }

"++"        { return TOKEN_INCREMENT; }
"--"        { return TOKEN_DECREMENT; }
"=="        { return TOKEN_EQUALITY; }
"!="        { return TOKEN_INEQUALITY; }
"<="        { return TOKEN_LESS_EQUAL; }
">="        { return TOKEN_GREATER_EQUAL; }
"&&"        { return TOKEN_LOGICAL_AND; }
"||"        { return TOKEN_LOGICAL_OR; }

 /* Tek karakterlik sembolleri ASCII değerleriyle doğrudan Bison'a döndürebiliriz */
[\[\](){}<>\+\-\*/\%\^\!\=\:\;\,] { return yytext[0]; }

\'[^\'\\]\' { return TOKEN_CHAR_LITERAL; }
\'\\n\'     { return TOKEN_CHAR_LITERAL; }
\'\\0\'     { return TOKEN_CHAR_LITERAL; }

{IDENTIFIER} { 
    yylval.name = strdup(yytext); 
    return TOKEN_IDENTIFIER; 
}

{DIGIT}+ { 
    yylval.name = strdup(yytext); 
    return TOKEN_INT_LITERAL; 
}

\"[^\"]*\" {
    /* Strip the leading and trailing quotes before sending to AST */
    yytext[yyleng-1] = '\0';
    yylval.name = strdup(yytext + 1);
    return TOKEN_STRING_LITERAL;
}

. { printf("Lexical Error: Unknown character '%s' at Line: %d\n", yytext, line_num); }

%%
