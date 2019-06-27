#include "./lex.h"
#include "./vec.h"
#include <stdlib.h>
#include <string.h>

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
  /* Node *expr; */
} VarDecl;

typedef struct {
  int type;

  union {
    TypeDecl *type_decl;
    FnCall *fn_call;
    VarDecl *var_decl;
  };
} Node;

TypeDecl *new_type_decl(char *name) {
  TypeDecl *type_decl = (TypeDecl *)malloc(sizeof(TypeDecl));
  type_decl->name = name;
  return type_decl;
}

FnCall *new_fn_call(char *name) {
  FnCall *fn_call = (FnCall *)malloc(sizeof(FnCall));
  fn_call->name = name;
  return fn_call;
}

VarDecl *new_var_decl(char *name) {
  VarDecl *var_decl = (VarDecl *)malloc(sizeof(VarDecl));
  var_decl->name = name;
  return var_decl;
}

Node *new_type_decl_node(char *name) {
  Node *node = (Node *)malloc(sizeof(Node));
  node->type_decl = new_type_decl(name);
  return node;
}

Node *new_fn_call_node(char *name) {
  Node *node = (Node *)malloc(sizeof(Node));
  node->fn_call = new_fn_call(name);
  return node;
}

Node *new_var_decl_node(char *name) {
  Node *node = (Node *)malloc(sizeof(Node));
  node->var_decl = new_var_decl(name);
  return node;
}

Parser *new_parser(char *source) {
  Parser *parser = (Parser *)malloc(sizeof(Parser));
  parser->lexer = lex(source);
  parser->pos = 0;
  parser->error = NULL;
  parser->nodes = new_vec();
  return parser;
}

void parser_add_node(Parser *parser, Node *node) {
  vec_push(parser->nodes, node);
}

Token *parser_tok(Parser *parser) { return parser->lexer->tokens[parser->pos]; }

bool parser_is_tok(Parser *parser, int type) {
  Token *tok = parser_tok(parser);
  return tok->type == type;
}

bool parser_is_ident(Parser *parser) {
  Token *tok = parser_tok(parser);
  return tok->type == TK_IDENT;
}

bool parser_is_ident_of(Parser *parser, char *ident) {
  Token *tok = parser_tok(parser);
  return tok->type == TK_IDENT && tok->val == ident;
}

bool parser_is_end(Parser *parser) {
  return parser->pos >= parser->lexer->tok_len || parser->error != NULL;
}

void parser_error(Parser *parser, char *message) { parser->error = message; }

Node *parser_type_decl(Parser *parser) {
  // consume "type"
  parser->pos++;
  if (!parser_is_ident(parser)) {
    parser_error(parser, "type name is expected after \"type\"");
    return NULL;
  }

  Token *tk_ident = parser_tok(parser);
  parser->pos++;

  if (!parser_is_tok(parser, ';')) {
    parser_error(parser, "\";\" is expected after type declaration");
    return NULL;
  }

  return new_type_decl_node(tk_ident->val);
}

Node *parser_fn_call(Parser *parser) {
  // consume function name
  Token *tk_ident = parser_tok(parser);
  parser->pos++;

  if (!parser_is_tok(parser, '(')) {
    parser_error(parser, "\"(\" is expcted after function name");
    return NULL;
  }

  // consume "("
  parser->pos++;

  if (!parser_is_tok(parser, ')')) {
    parser_error(parser, "\")\" is expcted after argument list");
    return NULL;
  }

  // consume ")"
  parser->pos++;

  return new_fn_call_node(tk_ident->val);
}

Node *parser_var_decl(Parser *parser) {
  Token *tk_ident = parser_tok(parser);

  // consume ident
  parser->pos++;

  if (!parser_is_tok(parser, '=')) {
    parser_error(parser, "\"=\" is expcted after variable name");
  }

  // consume "="
  parser->pos++;

  if (!parser_is_tok(parser, ';')) {
    parser_error(parser, "\";\" is expcted after variable declaration");
  }

  // consume ";"
  parser->pos++;

  return new_var_decl_node(tk_ident->val);
}

Parser *parse(char *source) {
  Parser *parser = new_parser(source);
  while (!parser_is_end(parser)) {

    if (parser_is_ident_of(parser, "type")) {
      // type declaration
      Node *type_decl = parser_type_decl(parser);
      if (type_decl != NULL) {
        parser_add_node(parser, type_decl);
      }
    } else if (parser_is_ident(parser)) {
      // function call or variable declaration
      parser->pos++;
      bool is_fn_call = parser_is_tok(parser, '(');
      parser->pos--;

      Node *node =
          is_fn_call ? parser_fn_call(parser) : parser_var_decl(parser);
      if (node != NULL) {
        parser_add_node(parser, node);
      }
    }
  }

  return parser;
}
