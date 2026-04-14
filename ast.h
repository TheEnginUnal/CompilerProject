#ifndef AST_H
#define AST_H

/* --- TYPES --- */
typedef enum {
    TYPE_VOID,
    TYPE_BOOLEAN,
    TYPE_CHAR,
    TYPE_INTEGER,
    TYPE_STRING
} type_kind_t;

struct type {
    type_kind_t kind;
    struct type *subtype; /* Used later for arrays/functions */
};

/* --- EXPRESSIONS --- */
typedef enum {
    EXPR_ASSIGN,
    EXPR_ADD,
    EXPR_SUB,
    EXPR_MUL,
    EXPR_DIV,
    EXPR_MOD,
    EXPR_EXP,
    EXPR_EQUAL,
    EXPR_NOT_EQUAL,
    EXPR_LESS,
    EXPR_GREATER,
    EXPR_LESS_EQUAL,
    EXPR_GREATER_EQUAL,
    EXPR_AND,
    EXPR_OR,
    EXPR_NOT,
    EXPR_INCREMENT,
    EXPR_DECREMENT,
    EXPR_NAME,         /* Identifier */
    EXPR_INTEGER_LIT,  /* E.g., 5 */
    EXPR_BOOLEAN_LIT,  /* true/false */
    EXPR_CHAR_LIT,     /* 'a' */
    EXPR_STRING_LIT    /* "hello" */
} expr_kind_t;

struct expr {
    expr_kind_t kind;
    struct expr *left;
    struct expr *right;
    const char *name;
    int integer_value;
    const char *string_literal;
    struct symbol *symbol;
};

/* --- STATEMENTS --- */
typedef enum {
    STMT_DECL,
    STMT_EXPR,
    STMT_IF_ELSE,
    STMT_FOR,
    STMT_PRINT,
    STMT_RETURN,
    STMT_BLOCK
} stmt_kind_t;

struct stmt {
    stmt_kind_t kind;
    struct decl *decl;
    struct expr *init_expr;
    struct expr *expr;
    struct expr *next_expr;
    struct stmt *body;
    struct stmt *else_body;
    struct stmt *next;
};

/* --- DECLARATIONS --- */
struct decl {
    const char *name;
    struct type *type;
    struct expr *value;
    struct stmt *code;
    struct decl *next;
    struct symbol *symbol;
};

/* --- FUNCTION PROTOTYPES (CONSTRUCTORS) --- */
struct type * type_create(type_kind_t kind, struct type *subtype);

struct expr * expr_create(expr_kind_t kind, struct expr *left, struct expr *right);
struct expr * expr_create_name(const char *name);
struct expr * expr_create_integer_literal(int c);
struct expr * expr_create_boolean_literal(int c);
struct expr * expr_create_char_literal(char c);
struct expr * expr_create_string_literal(const char *str);

struct stmt * stmt_create(stmt_kind_t kind, struct decl *decl, struct expr *init_expr, struct expr *expr, struct expr *next_expr, struct stmt *body, struct stmt *else_body, struct stmt *next);

struct decl * decl_create(const char *name, struct type *type, struct expr *value, struct stmt *code, struct decl *next);



/* --- PRINT FUNCTIONS (PRETTY PRINTER) --- */
void type_print(struct type *t);
void expr_print(struct expr *e);
void stmt_print(struct stmt *s, int indent);
void decl_print(struct decl *d, int indent);


#endif