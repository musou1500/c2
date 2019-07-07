#ifndef _AST_H
#define _AST_H

#include "./map.h"

enum BinopType {
  BO_LOGICAL_OR = 256,
  BO_LOGICAL_AND,
  BO_EQ,
  BO_NEQ,
  BO_LTE,
  BO_GTE,
  BO_INDEX
};

enum NodeType {
  ND_TYPE_DECL,
  ND_FN_CALL,
  ND_VAR_DECL,
  ND_STRING_LIT,
  ND_NUMBER_LIT,
  ND_ALLOC_EXPR,
  ND_ALLOC_ARRAY_EXPR,
  ND_BINOP_EXPR,
  ND_IDENT,
  ND_TYPE_SPEC
};

struct Node;

typedef struct {
  int type;
  struct Node *lhs;
  struct Node *rhs;
} BinopExpr;

typedef struct {
  char *name;
  Vec *params;
} TypeSpec;

typedef struct {
  char *name;
  Map *inits;
} AllocExpr;

typedef struct {
  char *name;
  struct Node *size_expr;
} AllocArrayExpr;

typedef struct {
  char *name;
} TypeDecl;

typedef struct {
  char *name;
  Vec *args;
} FnCall;

typedef struct {
  char *name;
  struct Node *expr;
  TypeSpec *type_spec;
} VarDecl;

typedef struct Node {
  int type;
  TypeSpec *type_spec;

  union {
    TypeDecl *type_decl;
    FnCall *fn_call;
    VarDecl *var_decl;
    AllocExpr *alloc_expr;
    AllocArrayExpr *alloc_array_expr;
    BinopExpr *binop_expr;
    char *str_lit;
    double num_lit;
    char *ident;
  };

} Node;

TypeDecl *new_type_decl(char *name);
FnCall *new_fn_call(char *name, Vec *args);
AllocExpr *new_alloc_expr(char *name, Map *inits);
AllocArrayExpr *new_alloc_array_expr(char *name, Node *size_expr);
VarDecl *new_var_decl(char *name, Node *expr);
BinopExpr *new_binop_expr(int type, Node *lhs, Node *rhs);
Node *new_type_decl_node(char *name);
Node *new_fn_call_node(char *name, Vec *args);
Node *new_var_decl_node(char *name, Node *expr);
Node *new_str_lit_node(char *val);
Node *new_num_lit_node(double val);
Node *new_alloc_expr_node(char *name, Map *inits);
Node *new_alloc_array_expr_node(char *name, Node *size_expr);
Node *new_binop_expr_node(int type, Node *lhs, Node *rhs);
Node *new_ident_node(char *name);
TypeSpec *new_type_spec(char *name, Vec *params);

#endif /* end of include guard */
