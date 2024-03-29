#ifndef _AST_H
#define _AST_H

#include "./map.h"
#include "./vec.h"

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
  ND_IF_STMT,
  ND_RET_STMT,
  ND_FN_DECL,
  ND_IMPORT
};

struct Node;

typedef struct {
  char *path;
  Vec *items;
} Import;

typedef struct IfStmt {
  struct Node *cond;
  Vec *stmts;
  struct Node *els;
} IfStmt;

typedef struct {
  struct Node *expr;
} RetStmt;

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
  Vec *args;
  TypeSpec *ret_type_spec;
  Vec *stmts;
  char *name;
} FnDecl;

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
    IfStmt *if_stmt;
    RetStmt *ret_stmt;
    FnDecl *fn_decl;
    Import *import;
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
IfStmt *new_if_stmt(Node *cond, Vec *stmts, Node *els);
RetStmt *new_ret_stmt(Node *expr);
Node *new_type_decl_node(char *name);
Node *new_fn_call_node(char *name, Vec *args);
Node *new_var_decl_node(char *name, Node *expr);
Node *new_str_lit_node(char *val);
Node *new_num_lit_node(double val);
Node *new_alloc_expr_node(char *name, Map *inits);
Node *new_alloc_array_expr_node(char *name, Node *size_expr);
Node *new_binop_expr_node(int type, Node *lhs, Node *rhs);
Node *new_ident_node(char *name);
Node *new_if_stmt_node(Node *cond, Vec *stmts, Node *els);
TypeSpec *new_type_spec(char *name, Vec *params);
Node *new_ret_stmt_node(Node *expr);
FnDecl *new_fn_decl(char *name, Vec *arg_names, TypeSpec *ret_type_spec, Vec *stmts);
Node *new_fn_decl_node(char *name, Vec *args, TypeSpec *ret_type_spec, Vec *stmts);
Import *new_import(char *path, Vec *items);
Node *new_import_node(char *path, Vec *items);

#endif /* end of include guard */
