#include <stdlib.h>
#include <string.h>
#include "scope.h"

/* A single entry in the symbol dictionary */
struct scope_node {
    char *name;
    struct symbol *sym;
    struct scope_node *next;
};

/* A stack of dictionaries (one for each scope level) */
struct scope_stack {
    struct scope_node *head; /* Linked list of symbols in this scope */
    struct scope_stack *next; /* Pointer to the outer scope */
};

/* The top of the stack points to the innermost current scope */
static struct scope_stack *stack_top = NULL;
static int current_level = -1;

void scope_enter() {
    struct scope_stack *new_scope = malloc(sizeof(*new_scope));
    new_scope->head = NULL;
    new_scope->next = stack_top;
    stack_top = new_scope;
    current_level++;
}

void scope_exit() {
    if (!stack_top) return;
    
    /* Clean up the linked list of the current scope */
    struct scope_node *curr = stack_top->head;
    while (curr) {
        struct scope_node *temp = curr;
        curr = curr->next;
        free(temp->name);
        free(temp);
    }
    
    /* Pop the stack */
    struct scope_stack *old_top = stack_top;
    stack_top = stack_top->next;
    free(old_top);
    current_level--;
}

int scope_level() {
    return current_level;
}

void scope_bind(const char *name, struct symbol *sym) {
    if (!stack_top) return;
    
    struct scope_node *new_node = malloc(sizeof(*new_node));
    new_node->name = strdup(name);
    new_node->sym = sym;
    
    new_node->next = stack_top->head;
    stack_top->head = new_node;
}

struct symbol * scope_lookup(const char *name) {
    struct scope_stack *curr_scope = stack_top;
    
    while (curr_scope) {
        struct scope_node *curr_node = curr_scope->head;
        while (curr_node) {
            if (strcmp(curr_node->name, name) == 0) {
                return curr_node->sym;
            }
            curr_node = curr_node->next;
        }
        curr_scope = curr_scope->next;
    }
    return NULL;
}

struct symbol * scope_lookup_current(const char *name) {
    if (!stack_top) return NULL;
    
    struct scope_node *curr_node = stack_top->head;
    while (curr_node) {
        if (strcmp(curr_node->name, name) == 0) {
            return curr_node->sym;
        }
        curr_node = curr_node->next;
    }
    return NULL;
}