#include <stdio.h>
#include <stdlib.h>
#include "resolve.h"
#include "scope.h"
#include "symbol.h"

int resolve_error_count = 0;

void expr_resolve(struct expr *e) {
    if (!e) return;
    
    if (e->kind == EXPR_NAME) {
        /* Check if the variable is defined anywhere in the active scopes */
        struct symbol *sym = scope_lookup(e->name);
        if (!sym) {
            printf("Resolve Error: Use of undeclared variable '%s'\n", e->name);
            resolve_error_count++;
        } else {
            /* Variable found! The compiler is happy. */
            printf("Resolved: Variable '%s' is valid.\n", e->name);
        }
    } else {
        /* Recursively check left and right sides of math/logic operations */
        expr_resolve(e->left);
        expr_resolve(e->right);
    }
}

void decl_resolve(struct decl *d) {
    if (!d) return;
    
    /* Check if the variable is ALREADY defined in the CURRENT scope */
    struct symbol *sym = scope_lookup_current(d->name);
    if (sym) {
        printf("Resolve Error: Variable '%s' is already defined in this scope\n", d->name);
        resolve_error_count++;
    } else {
        /* Create a new symbol and bind it to the dictionary */
        symbol_t kind = (scope_level() > 0) ? SYMBOL_LOCAL : SYMBOL_GLOBAL;
        sym = symbol_create(kind, d->type, d->name);
        scope_bind(d->name, sym);
        printf("Declared: '%s' added to Scope Level %d\n", d->name, scope_level());
    }
    
    /* Resolve the value it is initialized with (e.g., x = y + 5; check 'y') */
    expr_resolve(d->value);
    
    /* Move to the next declaration if there are multiples */
    decl_resolve(d->next);
}

void stmt_resolve(struct stmt *s) {
    if (!s) return;
    
    switch (s->kind) {
        case STMT_DECL:
            decl_resolve(s->decl);
            break;
        case STMT_EXPR:
        case STMT_PRINT:
        case STMT_RETURN:
            expr_resolve(s->expr);
            break;
        case STMT_BLOCK:
            scope_enter(); /* Open a new scope dictionary! */
            stmt_resolve(s->body);
            scope_exit();  /* Destroy the dictionary when leaving! */
            break;
        case STMT_IF_ELSE:
            expr_resolve(s->expr);
            stmt_resolve(s->body);
            stmt_resolve(s->else_body);
            break;
        default:
            break;
    }
    
    /* Recursively check the next statement */
    stmt_resolve(s->next);
}