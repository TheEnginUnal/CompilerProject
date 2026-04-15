#include <stdio.h>
#include <stdlib.h>
#include "typecheck.h"
#include "symbol.h"

int type_error_count = 0;

/* Check if two types are the exact same */
int type_equals(struct type *a, struct type *b) {
    if (!a || !b) return 0;
    if (a->kind == b->kind) return 1;
    return 0;
}

/* Creates a fresh copy of a type for the tree */
struct type * type_copy(struct type *t) {
    if (!t) return NULL;
    struct type *new_t = malloc(sizeof(*new_t));
    new_t->kind = t->kind;
    new_t->subtype = type_copy(t->subtype);
    return new_t;
}

void type_print_name(struct type *t) {
    if(!t) { printf("unknown"); return; }
    switch(t->kind) {
        case TYPE_INTEGER: printf("integer"); break;
        case TYPE_STRING:  printf("string"); break;
        case TYPE_BOOLEAN: printf("boolean"); break;
        case TYPE_CHAR:    printf("char"); break;
        case TYPE_VOID:    printf("void"); break;
    }
}

struct type * expr_typecheck(struct expr *e) {
    if (!e) return NULL;

    struct type *left_type = expr_typecheck(e->left);
    struct type *right_type = expr_typecheck(e->right);
    struct type *result_type = NULL;

    switch (e->kind) {
        case EXPR_INTEGER_LIT:
            result_type = type_create(TYPE_INTEGER, NULL);
            break;
        case EXPR_STRING_LIT:
            result_type = type_create(TYPE_STRING, NULL);
            break;
        case EXPR_BOOLEAN_LIT:
            result_type = type_create(TYPE_BOOLEAN, NULL);
            break;
        case EXPR_CHAR_LIT:
            result_type = type_create(TYPE_CHAR, NULL);
            break;
        case EXPR_NAME:
            /* Pull the type directly from the resolved symbol! */
            if (e->symbol) result_type = type_copy(e->symbol->type);
            break;

        /* Math requires both sides to be integers */
        case EXPR_ADD:
        case EXPR_SUB:
        case EXPR_MUL:
        case EXPR_DIV:
        case EXPR_MOD:
        case EXPR_EXP:
            if (left_type && right_type && (left_type->kind != TYPE_INTEGER || right_type->kind != TYPE_INTEGER)) {
                printf("Type Error: Cannot perform math on ");
                type_print_name(left_type); printf(" and "); type_print_name(right_type); printf("\n");
                type_error_count++;
            }
            result_type = type_create(TYPE_INTEGER, NULL);
            break;

        /* Comparisons (>, <, ==, !=, <=, >=) return boolean */
        case EXPR_EQUAL:
        case EXPR_NOT_EQUAL:
        case EXPR_LESS:
        case EXPR_GREATER:
        case EXPR_LESS_EQUAL:
        case EXPR_GREATER_EQUAL:
            if (left_type && right_type && !type_equals(left_type, right_type)) {
                printf("Type Error: Cannot compare ");
                type_print_name(left_type); printf(" with "); type_print_name(right_type); printf("\n");
                type_error_count++;
            }
            result_type = type_create(TYPE_BOOLEAN, NULL);
            break;

        /* Logic requires both sides to be boolean */
        case EXPR_AND:
        case EXPR_OR:
            if (left_type && right_type && (left_type->kind != TYPE_BOOLEAN || right_type->kind != TYPE_BOOLEAN)) {
                printf("Type Error: Logical operations require booleans.\n");
                type_error_count++;
            }
            result_type = type_create(TYPE_BOOLEAN, NULL);
            break;

        /* Assignments (x = 5) require left and right to match exactly */
        case EXPR_ASSIGN:
            if (left_type && right_type && !type_equals(left_type, right_type)) {
                printf("Type Error: Cannot assign ");
                type_print_name(right_type); printf(" to variable of type "); type_print_name(left_type); printf("\n");
                type_error_count++;
            }
            if (left_type) {
                result_type = type_copy(left_type);
            } else {
                result_type = type_create(TYPE_INTEGER, NULL);
            }
            break;
            
        default:
            result_type = type_create(TYPE_INTEGER, NULL); // Fallback
            break;
    }

    /* Clean up temp types */
    if(left_type) free(left_type);
    if(right_type) free(right_type);

    return result_type;
}

void decl_typecheck(struct decl *d) {
    if (!d) return;
    
    if (d->value) {
        struct type *val_type = expr_typecheck(d->value);
        if (d->type && val_type && !type_equals(d->type, val_type)) {
            printf("Type Error: Variable '%s' declared as ", d->name);
            type_print_name(d->type); printf(" but initialized with "); type_print_name(val_type); printf("\n");
            type_error_count++;
        }
        if(val_type) free(val_type);
    }
    decl_typecheck(d->next);
}

void stmt_typecheck(struct stmt *s) {
    if (!s) return;
    
    switch (s->kind) {
        case STMT_DECL:
            decl_typecheck(s->decl);
            break;
        case STMT_EXPR:
        case STMT_PRINT:
        case STMT_RETURN:
            expr_typecheck(s->expr);
            break;
        case STMT_BLOCK:
            stmt_typecheck(s->body);
            break;
        case STMT_IF_ELSE: {
            struct type *cond_type = expr_typecheck(s->expr);
            if (cond_type && cond_type->kind != TYPE_BOOLEAN) {
                printf("Type Error: 'if' condition must be a boolean.\n");
                type_error_count++;
            }
            if(cond_type) free(cond_type);
            stmt_typecheck(s->body);
            stmt_typecheck(s->else_body);
            break;
        }
        default: break;
    }
    stmt_typecheck(s->next);
}