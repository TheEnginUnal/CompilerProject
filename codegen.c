#include "codegen.h"
#include <stdlib.h>
#include <string.h>
#include "symbol.h"

FILE *outfile = NULL;

/* We will use 7 scratch registers for our math operations */
static int scratch_registers[7] = {0, 0, 0, 0, 0, 0, 0};
static const char *register_names[7] = {
    "%r10", "%r11", "%r12", "%r13", "%r14", "%r15", "%rbx"
};

static int label_counter = 0;

/* Finds an empty register, marks it as full, and returns its ID */
int scratch_alloc() {
    for (int i = 0; i < 7; i++) {
        if (scratch_registers[i] == 0) {
            scratch_registers[i] = 1; /* Mark as in-use */
            return i;
        }
    }
    fprintf(stderr, "CodeGen Error: Out of scratch registers!\n");
    exit(1);
}

/* Frees a register so it can be used again */
void scratch_free(int r) {
    if (r >= 0 && r < 7) {
        scratch_registers[r] = 0;
    }
}

/* Returns the actual x86-64 string name of the register (e.g., "%r10") */
const char * scratch_name(int r) {
    if (r >= 0 && r < 7) {
        return register_names[r];
    }
    return "UNKNOWN_REGISTER";
}

int label_create() {
    return label_counter++;
}

const char * label_name(int label) {
    static char buffer[32];
    sprintf(buffer, ".L%d", label);
    return buffer;
}

/* This will handle Global Variables */
void decl_codegen(struct decl *d) {
    if (!d) return;
    
    if (d->symbol && d->symbol->kind == SYMBOL_GLOBAL) {
        fprintf(outfile, ".data\n");
        fprintf(outfile, "%s:\n", d->name);
        
        if (d->type->kind == TYPE_INTEGER) {
            int val = d->value ? d->value->integer_value : 0;
            fprintf(outfile, "    .quad %d\n", val);
        } else if (d->type->kind == TYPE_STRING) {
            fprintf(outfile, "    .string \"%s\"\n", d->value ? d->value->string_literal : "");
        } else {
            fprintf(outfile, "    .quad 0\n");
        }
        fprintf(outfile, "\n");
    }
    
    decl_codegen(d->next);
}

/* We will fill these two functions in the next step! */
/* Translates Expressions into x86-64 Assembly */
int expr_codegen_x86(struct expr *e) {
    if (!e) return -1;
    
    int left_reg, right_reg;

    switch (e->kind) {
        case EXPR_INTEGER_LIT:
            left_reg = scratch_alloc();
            fprintf(outfile, "    movq $%d, %s\n", e->integer_value, scratch_name(left_reg));
            return left_reg;

        case EXPR_NAME:
            left_reg = scratch_alloc();
            /* Fetch variable from memory into a register */
            fprintf(outfile, "    movq %s, %s\n", e->name, scratch_name(left_reg));
            return left_reg;

        case EXPR_ASSIGN:
            right_reg = expr_codegen_x86(e->right);
            /* Save the register value back into memory */
            fprintf(outfile, "    movq %s, %s\n", scratch_name(right_reg), e->left->name);
            return right_reg;

        case EXPR_ADD:
            left_reg = expr_codegen_x86(e->left);
            right_reg = expr_codegen_x86(e->right);
            fprintf(outfile, "    addq %s, %s\n", scratch_name(right_reg), scratch_name(left_reg));
            scratch_free(right_reg);
            return left_reg;

        case EXPR_MUL:
            left_reg = expr_codegen_x86(e->left);
            right_reg = expr_codegen_x86(e->right);
            fprintf(outfile, "    imulq %s, %s\n", scratch_name(right_reg), scratch_name(left_reg));
            scratch_free(right_reg);
            return left_reg;

        case EXPR_GREATER:
            left_reg = expr_codegen_x86(e->left);
            right_reg = expr_codegen_x86(e->right);
            fprintf(outfile, "    cmpq %s, %s\n", scratch_name(right_reg), scratch_name(left_reg));
            fprintf(outfile, "    setg %%al\n");
            fprintf(outfile, "    movzbq %%al, %s\n", scratch_name(left_reg));
            scratch_free(right_reg);
            return left_reg;

        default:
            return -1;
    }
}

/* Translates Statements and Control Flow into x86-64 Assembly */
void stmt_codegen_x86(struct stmt *s) {
    if (!s) return;

    switch (s->kind) {
        case STMT_EXPR: {
            int r = expr_codegen_x86(s->expr);
            if (r >= 0) scratch_free(r);
            break;
        }
        case STMT_DECL:
            if (s->decl->value) {
                int r = expr_codegen_x86(s->decl->value);
                fprintf(outfile, "    movq %s, %s\n", scratch_name(r), s->decl->name);
                scratch_free(r);
            }
            break;
        case STMT_PRINT: {
            int r = expr_codegen_x86(s->expr);
            /* Call our custom C library function to print the integer! */
            fprintf(outfile, "    movq %s, %%rdi\n", scratch_name(r));
            fprintf(outfile, "    pushq %%r10\n"); /* Save caller-saved registers */
            fprintf(outfile, "    pushq %%r11\n");
            fprintf(outfile, "    call print_integer\n");
            fprintf(outfile, "    popq %%r11\n");
            fprintf(outfile, "    popq %%r10\n");
            scratch_free(r);
            break;
        }
        case STMT_BLOCK:
            stmt_codegen_x86(s->body);
            break;
        case STMT_IF_ELSE: {
            int false_label = label_create();
            int end_label = label_create();

            int cond_reg = expr_codegen_x86(s->expr);
            fprintf(outfile, "    cmpq $0, %s\n", scratch_name(cond_reg));
            fprintf(outfile, "    je %s\n", label_name(false_label)); /* Jump to ELSE if condition is 0 (false) */
            scratch_free(cond_reg);

            /* IF block */
            stmt_codegen_x86(s->body);
            fprintf(outfile, "    jmp %s\n", label_name(end_label));

            /* ELSE block */
            fprintf(outfile, "%s:\n", label_name(false_label));
            if (s->else_body) {
                stmt_codegen_x86(s->else_body);
            }

            fprintf(outfile, "%s:\n", label_name(end_label));
            break;
        }
        default: break;
    }
    stmt_codegen_x86(s->next);
}

/* Loops through the top-level statements to find Global Declarations */
void codegen_globals(struct stmt *s) {
    if (!s) return;
    
    if (s->kind == STMT_DECL) {
        decl_codegen(s->decl);
    }
    
    codegen_globals(s->next);
}