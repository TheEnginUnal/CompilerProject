#ifndef SCOPE_H
#define SCOPE_H

#include "symbol.h"

/* Enters a new scope block */
void scope_enter();

/* Exits the current scope block */
void scope_exit();

/* Returns the current scope level (0 = Global, 1+ = Local) */
int scope_level();

/* Adds a symbol to the current scope */
void scope_bind(const char *name, struct symbol *sym);

/* Searches for a symbol in ALL active scopes (from inner to outer) */
struct symbol * scope_lookup(const char *name);

/* Searches for a symbol ONLY in the current scope (used to detect re-declarations) */
struct symbol * scope_lookup_current(const char *name);

#endif