#include "./lex.h"
#include "./vec.h"
#include "./parse.h"
#include <stdlib.h>
#include <string.h>

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

VarDecl *new_var_decl(char *name, Node *expr) {
  VarDecl *var_decl = (VarDecl *)malloc(sizeof(VarDecl));
  var_decl->name = name;
  var_decl->expr = expr;
  return var_decl;
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

Parser *new_parser(Lexer *lexer) {
  Parser *parser = (Parser *)malloc(sizeof(Parser));
  parser->lexer = lexer;
  parser->pos = 0;
  parser->error = NULL;
  parser->nodes = new_vec();
  return parser;
}

void parser_add_node(Parser *parser, Node *node) {
  vec_push(parser->nodes, node);
}

Token *parser_tok(Parser *parser) {
  return (Token *)parser->lexer->tokens->data[parser->pos];
}

bool parser_is_type(Parser *parser, int type) {
  Token *tok = parser_tok(parser);
  return tok->type == type;
}

bool parser_is_ident(Parser *parser) {
  Token *tok = parser_tok(parser);
  return tok->type == TK_IDENT;
}

bool parser_is_ident_of(Parser *parser, char *ident) {
  Token *tok = parser_tok(parser);
  return tok->type == TK_IDENT && strcmp(tok->val, ident) == 0;
}

bool parser_is_end(Parser *parser) {
  return parser->pos >= parser->lexer->tokens->len || parser->error != NULL || parser->lexer->error != NULL;
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

  if (!parser_is_type(parser, ';')) {
    parser_error(parser, "\";\" is expected after type declaration");
    return NULL;
  }

  // consume ";"
  parser->pos++;

  return new_type_decl_node(tk_ident->val);
}

Node *parser_expr(Parser *parser);
Vec *parser_args(Parser* parser) {
  Vec *args = new_vec(); 
   
  do {
    Node *expr = parser_expr(parser);
    if(expr == NULL) {
      return NULL;
    }

    vec_push(args, expr);
  } while (parser_is_type(parser, ',') && parser->pos++);

  return args;
}


Node *parser_fn_call(Parser *parser) {
  // consume function name
  Token *tk_ident = parser_tok(parser);
  parser->pos++;

  if (!parser_is_type(parser, '(')) {
    parser_error(parser, "\"(\" is expcted after function name");
    return NULL;
  }

  // consume "("
  parser->pos++;
  Vec* args = parser_args(parser);

  if(args == NULL) {
    return NULL;
  }

  if (!parser_is_type(parser, ')')) {
    parser_error(parser, "\")\" is expcted after argument list");
    return NULL;
  }

  // consume ")"
  parser->pos++;

  return new_fn_call_node(tk_ident->val, args);
}

Node *parser_var_decl(Parser *parser) {
  Token *tk_ident = parser_tok(parser);
  // consume ident
  parser->pos++;

  if (!parser_is_type(parser, '=')) {
    parser_error(parser, "\"=\" is expcted after variable name");
    return NULL;
  }

  // consume "="
  parser->pos++;

  if (!parser_is_ident(parser)) {
    parser_error(parser, "identifier is expcted after \"=\"");
    return NULL;
  }

  Node *expr = parser_expr(parser);

  if (!parser_is_type(parser, ';')) {
    parser_error(parser, "\";\" is expcted after variable declaration");
    return NULL;
  }

  // consume ";"
  parser->pos++;

  return new_var_decl_node(tk_ident->val, expr);
}

Node *parser_expr(Parser *parser) {
  Token *tok = parser_tok(parser);
  if (parser_is_ident(parser)) {
    // function call or variable declaration
    parser->pos++;
    bool is_fn_call = parser_is_type(parser, '(');
    parser->pos--;

    Node *node =
      is_fn_call ? parser_fn_call(parser) : parser_var_decl(parser);
    if (node != NULL) {
      return node;
    }
  } else if (parser_is_type(parser, TK_NUMBER)) {
    parser->pos++;
    return new_num_lit_node(tok->n_val);
  } else if (parser_is_type(parser, TK_STRING)) {
    parser->pos++;
    return new_str_lit_node(tok->val);
  }

  parser_error(parser, "invalid expression");
  return NULL;
}

Parser *parse(char *source) {
  Lexer *lexer = lex(source);
  Parser *parser = new_parser(lexer);
  while (!parser_is_end(parser)) {
    if (parser_is_ident_of(parser, "type")) {
      // type declaration
      Node *type_decl = parser_type_decl(parser);
      if (type_decl != NULL) {
        parser_add_node(parser, type_decl);
      }
    } else {
      Node *expr = parser_expr(parser);
      parser_add_node(parser, expr);
    }
  }

  return parser;
}
