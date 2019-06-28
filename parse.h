#ifndef _PARSE_H
#define _PARSE_H

#include "./lex.h"
#include "./vec.h"

typedef struct {
  Lexer *lexer;
  Vec *nodes;
  int pos;
  char *error;
} Parser;

enum NodeType {
  ND_TYPE_DECL,
  ND_FN_CALL,
  ND_VAR_DECL,
  ND_STRING_LIT,
  ND_NUMBER_LIT,
};

typedef struct {
  char *name;
} TypeDecl;

typedef struct {
  char *name;
  Vec *args;
} FnCall;

struct Node;

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
    char *str_lit;
    double num_lit;
  };
} Node;

Parser *parse(char *source);

#endif
