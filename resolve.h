#ifndef RESOLVE_H
#define RESOLVE_H

#include "ast.h"

/* Tracks how many scope errors we find */
extern int resolve_error_count;

void decl_resolve(struct decl *d);
void stmt_resolve(struct stmt *s);
void expr_resolve(struct expr *e);

#endif