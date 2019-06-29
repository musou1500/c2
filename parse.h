#ifndef _PARSE_H
#define _PARSE_H

#include "./lex.h"
#include "./vec.h"
#include "./map.h"

typedef struct {
  Lexer *lexer;
  Vec *nodes;
  int pos;
  char *error;
} Parser;

enum BinopType {
  BO_LOGICAL_OR = 256,
  BO_LOGICAL_AND,
  BO_EQ,
  BO_NEQ,
  BO_LTE,
  BO_GTE,
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
};

struct Node;

typedef struct {
  int type;
  struct Node *lhs;
  struct Node *rhs;
} BinopExpr;

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
} VarDecl;

typedef struct Node {
  int type;

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

Parser *parse(char *source);
bool parser_has_error(Parser *parser);
void parser_print_error(Parser *parser);


#endif
