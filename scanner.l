/* --- 1. DEFINITIONS SECTION --- */
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Buffer for the rule: Strings in B-Minor can be a maximum of 256 characters */
char string_buffer[257]; 
int string_length = 0;
int line_num = 1;
%}

/* Flex options */
%option noyywrap
%option yylineno

/* Macros for B-Minor Rules */
DIGIT      [0-9]
LETTER     [a-zA-Z]
IDENTIFIER [a-zA-Z_][a-zA-Z0-9_]*

/* Exclusive states for multi-line comments and strings */
%x COMMENT_STATE
%x STRING_STATE

%%
 /* --- 2. RULES & ACTIONS SECTION --- */
 

 /* A2.1: Whitespace - Tabs, spaces, linefeed, and carriage return are ignored */
[ \t\r]+    { /* Do nothing */ }
\n          { line_num++; }

 /* A2.3: B-Minor Keywords */
"array"     { printf("KEYWORD: array\n"); }
"boolean"   { printf("KEYWORD: boolean\n"); }
"char"      { printf("KEYWORD: char\n"); }
"else"      { printf("KEYWORD: else\n"); }
"false"     { printf("KEYWORD: false\n"); }
"for"       { printf("KEYWORD: for\n"); }
"function"  { printf("KEYWORD: function\n"); }
"if"        { printf("KEYWORD: if\n"); }
"integer"   { printf("KEYWORD: integer\n"); }
"print"     { printf("KEYWORD: print\n"); }
"return"    { printf("KEYWORD: return\n"); }
"string"    { printf("KEYWORD: string\n"); }
"true"      { printf("KEYWORD: true\n"); }
"void"      { printf("KEYWORD: void\n"); }
"while"     { printf("KEYWORD: while\n"); }

 /* A4.1: Operators and Punctuation (Boşluksuz şekilde gruplandırıldı) */
[\[\](){}]  { printf("BRACKET: %s\n", yytext); }
"++"        { printf("OPERATOR (Increment): ++\n"); }
"--"        { printf("OPERATOR (Decrement): --\n"); }
"=="        { printf("OPERATOR (Equality): ==\n"); }
"!="        { printf("OPERATOR (Inequality): !=\n"); }
"<="        { printf("OPERATOR (Less or Equal): <=\n"); }
">="        { printf("OPERATOR (Greater or Equal): >=\n"); }
"&&"        { printf("OPERATOR (Logical AND): &&\n"); }
"||"        { printf("OPERATOR (Logical OR): ||\n"); }
[<>\+\-\*/\%\^\!\=] { printf("OPERATOR: %s\n", yytext); }
":"         { printf("TYPE DECLARATION: :\n"); }
";"         { printf("STATEMENT END: ;\n"); }
","         { printf("COMMA: ,\n"); }

 /* A3.2: Character Literal - Single quotes */
\'[^\'\\]\' { printf("CHAR: %s\n", yytext); }
\'\\n\'     { printf("CHAR (Escape): \\n\n"); }
\'\\0\'     { printf("CHAR (Escape): \\0\n"); }

 /* A2.2: Identifiers */
{IDENTIFIER} { 
    /* A3.3: Identifiers may be up to 256 characters long */
    if (yyleng > 256) {
        printf("ERROR: Identifier exceeds 256 characters! Line: %d\n", line_num);
    } else {
        printf("IDENTIFIER: %s\n", yytext); 
    }
}

 /* A3.2: Integers */
{DIGIT}+    { printf("INTEGER: %s\n", yytext); }

 /* A2.1: Single-line Comments (C++ Style) */
"//".* { /* Ignore single-line comment */ }

 /* A2.1: Multi-line Comments (C Style) */
"/*"        { BEGIN(COMMENT_STATE); }
<COMMENT_STATE>.|\n { 
    if (yytext[0] == '\n') line_num++; 
}
<COMMENT_STATE>"*/" { BEGIN(INITIAL); }

 /* A3.2 & A3.3: Strings */
\"          { 
    string_length = 0; 
    string_buffer[0] = '\0'; 
    BEGIN(STRING_STATE); 
}
<STRING_STATE>\" { 
    BEGIN(INITIAL);
    printf("STRING: \"%s\"\n", string_buffer);
}
<STRING_STATE>\\n { 
    if(string_length < 256) { string_buffer[string_length++] = '\n'; string_buffer[string_length] = '\0'; }
}
<STRING_STATE>\\0 { 
    if(string_length < 256) { string_buffer[string_length++] = '\0'; string_buffer[string_length] = '\0'; }
}
<STRING_STATE>\\\" { 
    if(string_length < 256) { string_buffer[string_length++] = '\"'; string_buffer[string_length] = '\0'; }
}
<STRING_STATE>\\\\ { 
    if(string_length < 256) { string_buffer[string_length++] = '\\'; string_buffer[string_length] = '\0'; }
}
<STRING_STATE>. { 
    if(string_length < 256) { 
        string_buffer[string_length++] = yytext[0]; 
        string_buffer[string_length] = '\0'; 
    } else {
        printf("ERROR: String exceeds 256 characters! Line: %d\n", line_num);
        BEGIN(INITIAL); /* Recover from error by returning to initial state */
    }
}

 /* Error Handling: Unknown characters */
.           { printf("ERROR: Unknown character '%s' at Line: %d\n", yytext, line_num); }

%%
/* --- 3. USER CODE SECTION --- */
int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            fprintf(stderr, "Could not open file: %s\n", argv[1]);
            return 1;
        }
        yyin = file;
    }
    yylex();
    return 0;
}