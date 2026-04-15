#ifndef IR_H
#define IR_H

#include "ast.h"

/* Generates Three-Address Code for a statement */
void stmt_codegen(struct stmt *s);

/* Generates Three-Address Code for an expression and returns the temp register ID */
int expr_codegen(struct expr *e);

/* Utility functions for generating unique temps and labels */
int ir_create_temp();
int ir_create_label();

#endif