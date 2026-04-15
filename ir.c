#include <stdio.h>
#include <stdlib.h>
#include "ir.h"

/* Global counters to ensure unique temporary variables and labels */
static int temp_count = 0;
static int label_count = 0;

int ir_create_temp() {
    return temp_count++;
}

int ir_create_label() {
    return label_count++;
}

int expr_codegen(struct expr *e) {
    if (!e) return -1;

    int left_temp, right_temp, result_temp;

    switch (e->kind) {
        /* Base Cases: Load values into temporary variables */
        case EXPR_INTEGER_LIT:
            result_temp = ir_create_temp();
            printf("    t%d = %d\n", result_temp, e->integer_value);
            return result_temp;

        case EXPR_STRING_LIT:
            result_temp = ir_create_temp();
            printf("    t%d = \"%s\"\n", result_temp, e->string_literal);
            return result_temp;

        case EXPR_BOOLEAN_LIT:
            result_temp = ir_create_temp();
            printf("    t%d = %s\n", result_temp, e->integer_value ? "true" : "false");
            return result_temp;

        case EXPR_NAME:
            result_temp = ir_create_temp();
            printf("    t%d = %s\n", result_temp, e->name);
            return result_temp;

        /* Assignments */
        case EXPR_ASSIGN:
            right_temp = expr_codegen(e->right);
            printf("    %s = t%d\n", e->left->name, right_temp);
            return right_temp;

        /* Math Operations (3-Address Code Style) */
        case EXPR_ADD:
            left_temp = expr_codegen(e->left);
            right_temp = expr_codegen(e->right);
            result_temp = ir_create_temp();
            printf("    t%d = t%d + t%d\n", result_temp, left_temp, right_temp);
            return result_temp;

        case EXPR_SUB:
            left_temp = expr_codegen(e->left);
            right_temp = expr_codegen(e->right);
            result_temp = ir_create_temp();
            printf("    t%d = t%d - t%d\n", result_temp, left_temp, right_temp);
            return result_temp;

        case EXPR_MUL:
            left_temp = expr_codegen(e->left);
            right_temp = expr_codegen(e->right);
            result_temp = ir_create_temp();
            printf("    t%d = t%d * t%d\n", result_temp, left_temp, right_temp);
            return result_temp;

        case EXPR_GREATER:
            left_temp = expr_codegen(e->left);
            right_temp = expr_codegen(e->right);
            result_temp = ir_create_temp();
            printf("    t%d = t%d > t%d\n", result_temp, left_temp, right_temp);
            return result_temp;

        default:
            return -1;
    }
}

void stmt_codegen(struct stmt *s) {
    if (!s) return;

    switch (s->kind) {
        case STMT_DECL:
            if (s->decl->value) {
                int val_temp = expr_codegen(s->decl->value);
                printf("    %s = t%d\n", s->decl->name, val_temp);
            }
            break;

        case STMT_EXPR:
            expr_codegen(s->expr);
            break;

        case STMT_PRINT: {
            int print_temp = expr_codegen(s->expr);
            printf("    PRINT t%d\n", print_temp);
            break;
        }

        case STMT_BLOCK:
            stmt_codegen(s->body);
            break;

        /* Control Flow: IF/ELSE with Labels! */
        case STMT_IF_ELSE: {
            int cond_temp = expr_codegen(s->expr);
            int false_label = ir_create_label();
            int end_label = ir_create_label();

            printf("    JMP_FALSE t%d, .L%d\n", cond_temp, false_label);

            /* IF block */
            stmt_codegen(s->body);
            printf("    JMP .L%d\n", end_label);

            /* ELSE block */
            printf(".L%d:\n", false_label);
            if (s->else_body) {
                stmt_codegen(s->else_body);
            }

            printf(".L%d:\n", end_label);
            break;
        }
        
        default: break;
    }
    
    stmt_codegen(s->next);
}