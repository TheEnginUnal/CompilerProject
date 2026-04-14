#include "ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Creates a type node */
struct type * type_create(type_kind_t kind, struct type *subtype) {
    struct type *t = malloc(sizeof(*t));
    t->kind = kind;
    t->subtype = subtype;
    return t;
}

/* Creates a general expression node (like Math or Logic) */
struct expr * expr_create(expr_kind_t kind, struct expr *left, struct expr *right) {
    struct expr *e = malloc(sizeof(*e));
    e->kind = kind;
    e->left = left;
    e->right = right;
    e->name = NULL;
    e->string_literal = NULL;
    return e;
}

/* Creates a variable name expression */
struct expr * expr_create_name(const char *name) {
    struct expr *e = expr_create(EXPR_NAME, NULL, NULL);
    e->name = strdup(name);
    return e;
}

/* Creates an integer literal expression */
struct expr * expr_create_integer_literal(int c) {
    struct expr *e = expr_create(EXPR_INTEGER_LIT, NULL, NULL);
    e->integer_value = c;
    return e;
}

/* Creates a string literal expression */
struct expr * expr_create_string_literal(const char *str) {
    struct expr *e = expr_create(EXPR_STRING_LIT, NULL, NULL);
    e->string_literal = strdup(str);
    return e;
}

/* Creates a statement node */
struct stmt * stmt_create(stmt_kind_t kind, struct decl *decl, struct expr *init_expr, struct expr *expr, struct expr *next_expr, struct stmt *body, struct stmt *else_body, struct stmt *next) {
    struct stmt *s = malloc(sizeof(*s));
    s->kind = kind;
    s->decl = decl;
    s->init_expr = init_expr;
    s->expr = expr;
    s->next_expr = next_expr;
    s->body = body;
    s->else_body = else_body;
    s->next = next;
    return s;
}

/* Creates a declaration node */
struct decl * decl_create(const char *name, struct type *type, struct expr *value, struct stmt *code, struct decl *next) {
    struct decl *d = malloc(sizeof(*d));
    d->name = strdup(name);
    d->type = type;
    d->value = value;
    d->code = code;
    d->next = next;
    return d;
}

/* Creates a boolean literal expression (true/false) */
struct expr * expr_create_boolean_literal(int c) {
    struct expr *e = expr_create(EXPR_BOOLEAN_LIT, NULL, NULL);
    e->integer_value = c;
    return e;
}

/* --- AST PRINTING IMPLEMENTATION --- */

/* Helper function to manage indentation */
void print_indent(int indent) {
    for (int i = 0; i < indent; i++) {
        printf("    ");
    }
}

void type_print(struct type *t) {
    if (!t) return;
    switch(t->kind) {
        case TYPE_VOID:    printf("void"); break;
        case TYPE_BOOLEAN: printf("boolean"); break;
        case TYPE_CHAR:    printf("char"); break;
        case TYPE_INTEGER: printf("integer"); break;
        case TYPE_STRING:  printf("string"); break;
    }
}

void expr_print(struct expr *e) {
    if (!e) return;
    switch(e->kind) {
        case EXPR_NAME: 
            printf("%s", e->name); 
            break;
        case EXPR_INTEGER_LIT: 
            printf("%d", e->integer_value); 
            break;
        case EXPR_BOOLEAN_LIT: 
            printf("%s", e->integer_value ? "true" : "false"); 
            break;
        case EXPR_STRING_LIT: 
            printf("\"%s\"", e->string_literal); 
            break;
        case EXPR_ASSIGN: 
            expr_print(e->left); printf(" = "); expr_print(e->right); 
            break;
        
        /* Math & Logic Operations (Wrapped in parentheses to show tree structure) */
        case EXPR_ADD: printf("("); expr_print(e->left); printf(" + "); expr_print(e->right); printf(")"); break;
        case EXPR_SUB: printf("("); expr_print(e->left); printf(" - "); expr_print(e->right); printf(")"); break;
        case EXPR_MUL: printf("("); expr_print(e->left); printf(" * "); expr_print(e->right); printf(")"); break;
        case EXPR_DIV: printf("("); expr_print(e->left); printf(" / "); expr_print(e->right); printf(")"); break;
        case EXPR_MOD: printf("("); expr_print(e->left); printf(" %% "); expr_print(e->right); printf(")"); break;
        case EXPR_EXP: printf("("); expr_print(e->left); printf(" ^ "); expr_print(e->right); printf(")"); break;
        case EXPR_EQUAL: printf("("); expr_print(e->left); printf(" == "); expr_print(e->right); printf(")"); break;
        case EXPR_NOT_EQUAL: printf("("); expr_print(e->left); printf(" != "); expr_print(e->right); printf(")"); break;
        case EXPR_LESS: printf("("); expr_print(e->left); printf(" < "); expr_print(e->right); printf(")"); break;
        case EXPR_GREATER: printf("("); expr_print(e->left); printf(" > "); expr_print(e->right); printf(")"); break;
        case EXPR_LESS_EQUAL: printf("("); expr_print(e->left); printf(" <= "); expr_print(e->right); printf(")"); break;
        case EXPR_GREATER_EQUAL: printf("("); expr_print(e->left); printf(" >= "); expr_print(e->right); printf(")"); break;
        case EXPR_AND: printf("("); expr_print(e->left); printf(" && "); expr_print(e->right); printf(")"); break;
        case EXPR_OR: printf("("); expr_print(e->left); printf(" || "); expr_print(e->right); printf(")"); break;
        default: break;
    }
}

void decl_print(struct decl *d, int indent) {
    if (!d) return;
    print_indent(indent);
    printf("%s: ", d->name);
    type_print(d->type);
    
    if (d->value) {
        printf(" = ");
        expr_print(d->value);
    }
    printf(";\n");
}

void stmt_print(struct stmt *s, int indent) {
    if (!s) return;
    
    switch(s->kind) {
        case STMT_DECL:
            decl_print(s->decl, indent);
            break;
        case STMT_EXPR:
            print_indent(indent);
            expr_print(s->expr);
            printf(";\n");
            break;
        case STMT_PRINT:
            print_indent(indent);
            printf("print ");
            expr_print(s->expr);
            printf(";\n");
            break;
        case STMT_RETURN:
            print_indent(indent);
            printf("return ");
            expr_print(s->expr);
            printf(";\n");
            break;
        case STMT_BLOCK:
            printf("{\n");
            struct stmt *curr = s->body;
            while(curr) {
                stmt_print(curr, indent + 1);
                curr = curr->next;
            }
            print_indent(indent);
            printf("}\n");
            break;
        case STMT_IF_ELSE:
            print_indent(indent);
            printf("if (");
            expr_print(s->expr);
            printf(") ");
            
            if (s->body && s->body->kind == STMT_BLOCK) {
                stmt_print(s->body, indent);
            } else {
                printf("\n");
                stmt_print(s->body, indent + 1);
            }
            
            if (s->else_body) {
                print_indent(indent);
                printf("else ");
                if (s->else_body->kind == STMT_BLOCK) {
                    stmt_print(s->else_body, indent);
                } else {
                    printf("\n");
                    stmt_print(s->else_body, indent + 1);
                }
            }
            break;
        default: break;
    }
    
    /* Recursively print the next statement in the tree */
    stmt_print(s->next, indent); 
}