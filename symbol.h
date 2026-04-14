#ifndef SYMBOL_H
#define SYMBOL_H

#include "ast.h"

typedef enum {
    SYMBOL_LOCAL,
    SYMBOL_GLOBAL,
    SYMBOL_PARAM
} symbol_t;

struct symbol {
    symbol_t kind;
    struct type *type;
    char *name;
    int which; /* To track variable memory offset later */
};

struct symbol * symbol_create(symbol_t kind, struct type *type, char *name);

#endif