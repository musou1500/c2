#include <stdlib.h>
#include "./ast.h"
#include "./vec.h"

TypeDecl *new_type_decl(char *name) {
  TypeDecl *type_decl = (TypeDecl *)malloc(sizeof(TypeDecl));
  type_decl->name = name;
  return type_decl;
}

FnCall *new_fn_call(char *name, Vec *args) {
  FnCall *fn_call = (FnCall *)malloc(sizeof(FnCall));
  fn_call->name = name;
  fn_call->args = args;
  return fn_call;
}

AllocExpr *new_alloc_expr(char *name, Map *inits) {
  AllocExpr *alloc_expr = (AllocExpr *)malloc(sizeof(AllocExpr));
  alloc_expr->name = name;
  alloc_expr->inits = inits;
  return alloc_expr;
}

AllocArrayExpr *new_alloc_array_expr(char *name, Node *size_expr) {
  AllocArrayExpr *alloc_array_expr =
      (AllocArrayExpr *)malloc(sizeof(AllocArrayExpr));
  alloc_array_expr->name = name;
  alloc_array_expr->size_expr = size_expr;
  return alloc_array_expr;
}

VarDecl *new_var_decl(char *name, Node *expr) {
  VarDecl *var_decl = (VarDecl *)malloc(sizeof(VarDecl));
  var_decl->name = name;
  var_decl->expr = expr;
  return var_decl;
}

BinopExpr *new_binop_expr(int type, Node *lhs, Node *rhs) {
  BinopExpr *binop_expr = (BinopExpr *)malloc(sizeof(BinopExpr));
  binop_expr->type = type;
  binop_expr->lhs = lhs;
  binop_expr->rhs = rhs;
  return binop_expr;
}

Node *new_type_decl_node(char *name) {
  Node *node = (Node *)malloc(sizeof(Node));
  node->type = ND_TYPE_DECL;
  node->type_decl = new_type_decl(name);
  return node;
}

Node *new_fn_call_node(char *name, Vec *args) {
  Node *node = (Node *)malloc(sizeof(Node));
  node->type = ND_FN_CALL;
  node->fn_call = new_fn_call(name, args);
  return node;
}

Node *new_var_decl_node(char *name, Node *expr) {
  Node *node = (Node *)malloc(sizeof(Node));
  node->type = ND_VAR_DECL;
  node->var_decl = new_var_decl(name, expr);
  return node;
}

Node *new_str_lit_node(char *val) {
  Node *node = (Node *)malloc(sizeof(Node));
  node->type = ND_STRING_LIT;
  node->str_lit = val;
  return node;
}

Node *new_num_lit_node(double val) {
  Node *node = (Node *)malloc(sizeof(Node));
  node->type = ND_NUMBER_LIT;
  node->num_lit = val;
  return node;
}

Node *new_alloc_expr_node(char *name, Map *inits) {
  Node *node = (Node *)malloc(sizeof(Node));
  node->type = ND_ALLOC_EXPR;
  node->alloc_expr = new_alloc_expr(name, inits);
  return node;
}

Node *new_alloc_array_expr_node(char *name, Node *size_expr) {
  Node *node = (Node *)malloc(sizeof(Node));
  node->type = ND_ALLOC_ARRAY_EXPR;
  node->alloc_array_expr = new_alloc_array_expr(name, size_expr);
  return node;
}

Node *new_binop_expr_node(int type, Node *lhs, Node *rhs) {
  Node *node = (Node *)malloc(sizeof(Node));
  node->type = ND_BINOP_EXPR;
  node->binop_expr = new_binop_expr(type, lhs, rhs);
  return node;
}

Node *new_ident_node(char *name) {
  Node *node = (Node *)malloc(sizeof(Node));
  node->type = ND_IDENT;
  node->ident = name;
  return node;
}

TypeSpec *new_type_spec(char *name, Vec *params) {
  TypeSpec *type_spec = (TypeSpec *) malloc(sizeof(TypeSpec));
  type_spec->name = name;
  type_spec->params = params;
  return type_spec;
}

IfStmt *new_if_stmt(Node *cond, Vec *stmts, Node *els) {
  IfStmt *if_stmt = (IfStmt *)malloc(sizeof(IfStmt));
  if_stmt->stmts = stmts;
  if_stmt->els = els;
  if_stmt->cond = cond;
  return if_stmt;
}

Node *new_if_stmt_node(Node *cond, Vec *stmts, Node *els) {
  Node *node = (Node *)malloc(sizeof(Node));
  node->type = ND_IF_STMT;
  node->if_stmt = new_if_stmt(cond, stmts, els);
  return node;
}

RetStmt *new_ret_stmt(Node *expr) {
  RetStmt *stmt = (RetStmt *)malloc(sizeof(RetStmt));
  stmt->expr = expr;
  return stmt;
}

Node *new_ret_stmt_node(Node *expr) {
  Node *node = (Node *)malloc(sizeof(Node));
  node->type = ND_RET_STMT;
  node->ret_stmt = new_ret_stmt(expr);
  return node;
}

FnDecl *new_fn_decl(char *name, Vec *args, TypeSpec *ret_type_spec, Vec *stmts) {
  FnDecl *fn_decl = (FnDecl *)malloc(sizeof(FnDecl));
  fn_decl->name = name;
  fn_decl->args = args;
  fn_decl->stmts = stmts;
  fn_decl->ret_type_spec = ret_type_spec;
  return fn_decl;
}

Node *new_fn_decl_node(char *name, Vec *args, TypeSpec *ret_type_spec, Vec *stmts) {
  Node *node = (Node *)malloc(sizeof(Node));
  node->type = ND_FN_DECL;
  node->fn_decl = new_fn_decl(name, args, ret_type_spec, stmts);
  return node;
}
