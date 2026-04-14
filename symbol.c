#include <stdlib.h>
#include <string.h>
#include "symbol.h"

struct symbol * symbol_create(symbol_t kind, struct type *type, char *name) {
    struct symbol *sym = malloc(sizeof(*sym));
    sym->kind = kind;
    sym->type = type;
    sym->name = strdup(name);
    sym->which = 0;
    return sym;
}