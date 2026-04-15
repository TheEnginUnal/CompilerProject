#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include <stdio.h>

/* The output file where we will write our Assembly code */
extern FILE *outfile;

/* Main entry points for Code Generation */
void decl_codegen(struct decl *d);
void stmt_codegen_x86(struct stmt *s);
int expr_codegen_x86(struct expr *e);
/* Extracts global variables from the main AST */
void codegen_globals(struct stmt *s);

/* Register Management Functions */
int scratch_alloc();
void scratch_free(int r);
const char * scratch_name(int r);

/* Label Management */
int label_create();
const char * label_name(int label);

#endif