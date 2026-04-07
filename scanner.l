%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Bison'un ürettiği Jeton kimliklerini (Token ID) içeren başlık dosyasını dahil ediyoruz! */
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
%x STRING_STATE

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

{IDENTIFIER} { return TOKEN_IDENTIFIER; }
{DIGIT}+    { return TOKEN_INT_LITERAL; }

"//".* { /* Ignore single-line comment */ }

"/*"        { BEGIN(COMMENT_STATE); }
<COMMENT_STATE>.|\n { if (yytext[0] == '\n') line_num++; }
<COMMENT_STATE>"*/" { BEGIN(INITIAL); }

\"          { string_length = 0; string_buffer[0] = '\0'; BEGIN(STRING_STATE); }
<STRING_STATE>\" { BEGIN(INITIAL); return TOKEN_STRING_LITERAL; }
<STRING_STATE>\\n { if(string_length < 256) { string_buffer[string_length++] = '\n'; string_buffer[string_length] = '\0'; } }
<STRING_STATE>\\0 { if(string_length < 256) { string_buffer[string_length++] = '\0'; string_buffer[string_length] = '\0'; } }
<STRING_STATE>\\\" { if(string_length < 256) { string_buffer[string_length++] = '\"'; string_buffer[string_length] = '\0'; } }
<STRING_STATE>\\\\ { if(string_length < 256) { string_buffer[string_length++] = '\\'; string_buffer[string_length] = '\0'; } }
<STRING_STATE>. { 
    if(string_length < 256) { 
        string_buffer[string_length++] = yytext[0]; 
        string_buffer[string_length] = '\0'; 
    } else {
        BEGIN(INITIAL); 
    }
}

. { printf("Lexical Error: Unknown character '%s' at Line: %d\n", yytext, line_num); }

%%
/* main() fonksiyonunu parser.y dosyasına taşıdığımız için burayı tamamen boş bıraktık. */