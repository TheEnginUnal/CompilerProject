#ifndef TYPECHECK_H
#define TYPECHECK_H

#include "ast.h"

extern int type_error_count;

struct type * expr_typecheck(struct expr *e);
void stmt_typecheck(struct stmt *s);
void decl_typecheck(struct decl *d);

/* Utility functions */
int type_equals(struct type *a, struct type *b);
struct type * type_copy(struct type *t);
void type_print_name(struct type *t);

#endif