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
};

typedef struct {
  char *name;
} TypeDecl;

typedef struct {
  char *name;
} FnCall;

typedef struct {
  char *name;
  char *ident;
} VarDecl;

typedef struct {
  int type;

  union {
    TypeDecl *type_decl;
    FnCall *fn_call;
    VarDecl *var_decl;
  };
} Node;

Parser *parse(char *source);
