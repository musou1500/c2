#include "./parse.h"
#include "./lex.h"
#include "./map.h"
#include "./vec.h"
#include <stdio.h>
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

bool tok_is_type(Token *tok, int type) { return tok->type == type; }

bool parser_is_type(Parser *parser, int type) {
  return tok_is_type(parser_tok(parser), type);
}

bool parser_is_ident(Parser *parser) {
  return tok_is_type(parser_tok(parser), TK_IDENT);
}

bool parser_is_ident_of(Parser *parser, char *ident) {
  Token *tok = parser_tok(parser);
  return tok_is_type(tok, TK_IDENT) && strcmp(tok->val, ident) == 0;
}

bool parser_has_error(Parser *parser) {
  return parser->error != NULL || lex_has_error(parser->lexer);
}

bool parser_is_end(Parser *parser) {
  return parser_is_type(parser, TK_EOF) || parser_has_error(parser);
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

  return new_type_decl_node(tk_ident->val);
}

Node *parser_expr(Parser *parser);
Vec *parser_args(Parser *parser) {
  Vec *args = new_vec();

  if (parser_is_type(parser, ')')) {
    return args;
  }

  do {
    Node *expr = parser_expr(parser);
    if (expr == NULL) {
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
  Vec *args = parser_args(parser);

  if (parser_has_error(parser)) {
    return NULL;
  }

  if (!parser_is_type(parser, ')')) {
    parser_error(parser, "\")\" is expected after argument list");
    return NULL;
  }

  // consume ")"
  parser->pos++;

  return new_fn_call_node(tk_ident->val, args);
}

Node *parser_var_decl(Parser *parser) {
  // consume "var"
  parser->pos++;
  Token *tk_ident = parser_tok(parser);
  // consume ident
  parser->pos++;

  if (!parser_is_type(parser, '=')) {
    parser_error(parser, "\"=\" is expected after variable name");
    return NULL;
  }

  // consume "="
  parser->pos++;

  Node *expr = parser_expr(parser);
  if (parser_has_error(parser)) {
    return NULL;
  }

  return new_var_decl_node(tk_ident->val, expr);
}

Map *parser_alloc_inits(Parser *parser) {
  // consume "{"
  parser->pos++;

  Map *inits = new_map();

  do {
    if (!parser_is_type(parser, '.')) {
      parser_error(parser, "\".\" is expetected before initializer");
      return NULL;
    }

    // consume "."
    parser->pos++;

    if (!parser_is_ident(parser)) {
      parser_error(parser, "identifier is expetected for lhs of initializer");
      return NULL;
    }

    Token *ident_tok = parser_tok(parser);

    // consume ident
    parser->pos++;

    if (!parser_is_type(parser, '=')) {
      parser_error(parser,
                   "\"=\" is exptected after field name of initializer");
      return NULL;
    }

    // consume "="
    parser->pos++;

    Node *expr = parser_expr(parser);
    if (parser_has_error(parser)) {
      return NULL;
    }

    map_put(inits, ident_tok->val, expr);
  } while (parser_is_type(parser, ',') && parser->pos++);

  return inits;
}

Node *parser_alloc_expr(Parser *parser) {
  // consume "alloc"
  parser->pos++;

  if (!parser_is_ident(parser)) {
    parser_error(parser, "type name is expected after \"alloc\"");
    return NULL;
  }

  Token *type_tok = parser_tok(parser);
  parser->pos++;

  // alloc array expr
  if (parser_is_type(parser, '[')) {
    parser->pos++;
    Node *size_expr = parser_expr(parser);
    if (parser_has_error(parser)) {
      return NULL;
    }

    if (!parser_is_type(parser, ']')) {
      parser_error(parser, "\"]\" is expected after size of alloc array expr");
      return NULL;
    }

    parser->pos++;

    return new_alloc_array_expr_node(type_tok->val, size_expr);
  }

  if (!parser_is_type(parser, '{')) {
    parser_error(parser, "\"{\" is expected after typename");
    return NULL;
  }

  Map *inits = parser_alloc_inits(parser);
  if (inits == NULL) {
    return NULL;
  }

  if (!parser_is_type(parser, '}')) {
    parser_error(parser, "\"}\" is expected after inits");
    return NULL;
  }

  // consume "}"
  parser->pos++;

  return new_alloc_expr_node(type_tok->val, inits);
}

Node *parser_term(Parser *parser) {
  Token *tok = parser_tok(parser);

  // alloc expr
  if (parser_is_ident_of(parser, "alloc")) {
    return parser_alloc_expr(parser);
  }

  // function call or identifier
  if (parser_is_ident(parser)) {
    parser->pos++;
    Token *next_tok = parser_tok(parser);
    parser->pos--;
    if (tok_is_type(next_tok, '(')) {
      return parser_fn_call(parser);
    } else {
      parser->pos++;
      return new_ident_node(tok->val);
    }
  }

  // literal
  if (parser_is_type(parser, TK_NUMBER)) {
    parser->pos++;
    return new_num_lit_node(tok->n_val);
  } else if (parser_is_type(parser, TK_STRING)) {
    parser->pos++;
    return new_str_lit_node(tok->val);
  }

  if (parser_is_type(parser, '(')) {
    parser->pos++;
    Node *node = parser_expr(parser);
    if (!parser_is_type(parser, ')')) {
      parser_error(parser, "parentheses is not closed");
      return NULL;
    }

    parser->pos++;
    return node;
  }

  parser_error(parser, "unexpected token");
  return NULL;
}

Node *parser_mult_div(Parser *parser) {
  Node *lhs = parser_term(parser);

  if (parser_is_type(parser, '*')) {
    parser->pos++;
    return new_binop_expr_node('*', lhs, parser_mult_div(parser));
  }

  if (parser_is_type(parser, '/')) {
    parser->pos++;
    return new_binop_expr_node('/', lhs, parser_mult_div(parser));
  }

  return lhs;
}

Node *parser_add_sub(Parser *parser) {
  Node *lhs = parser_mult_div(parser);
  if (parser_is_type(parser, '+')) {
    parser->pos++;
    return new_binop_expr_node('+', lhs, parser_add_sub(parser));
  }

  if (parser_is_type(parser, '-')) {
    parser->pos++;
    return new_binop_expr_node('-', lhs, parser_add_sub(parser));
  }

  return lhs;
}

Node *parser_cmp(Parser *parser) {
  Node *lhs = parser_add_sub(parser);

  if (parser_is_type(parser, TK_NEQ)) {
    parser->pos++;
    return new_binop_expr_node(BO_NEQ, lhs, parser_cmp(parser));
  }

  if (parser_is_type(parser, TK_EQ)) {
    parser->pos++;
    return new_binop_expr_node(BO_EQ, lhs, parser_cmp(parser));
  }

  if (parser_is_type(parser, '>')) {
    parser->pos++;
    return new_binop_expr_node('>', lhs, parser_cmp(parser));
  }

  if (parser_is_type(parser, TK_GTE)) {
    parser->pos++;
    return new_binop_expr_node(BO_GTE, lhs, parser_cmp(parser));
  }

  if (parser_is_type(parser, '<')) {
    parser->pos++;
    return new_binop_expr_node('<', lhs, parser_cmp(parser));
  }

  if (parser_is_type(parser, TK_LTE)) {
    parser->pos++;
    return new_binop_expr_node(BO_LTE, lhs, parser_cmp(parser));
  }

  return lhs;
}

// logical_and: parser_cmp logical_and'
// logical_and': ε | "&&" logical
Node *parser_logical_and(Parser *parser) {
  Node *lhs = parser_cmp(parser);
  if (parser_is_type(parser, TK_LOGICAL_AND)) {
    parser->pos++;
    return new_binop_expr_node(BO_LOGICAL_AND, lhs, parser_expr(parser));
  }

  return lhs;
}

// logical: logical_and logical'
// logical': ε | "||" logical_and
Node *parser_expr(Parser *parser) {
  Node *lhs = parser_logical_and(parser);
  if (parser_is_type(parser, TK_LOGICAL_OR)) {
    parser->pos++;
    return new_binop_expr_node(BO_LOGICAL_OR, lhs, parser_logical_and(parser));
  }

  return lhs;
}

Node *parser_stmt(Parser *parser) {
  Node *stmt;
  if (parser_is_ident_of(parser, "var")) {
    stmt = parser_var_decl(parser);
  } else if (parser_is_ident_of(parser, "type")) {
    stmt = parser_type_decl(parser);
  } else {
    stmt = parser_expr(parser);
  }

  if (!parser_is_type(parser, ';')) {
    parser_error(parser, "\";\" is expected after statement");
    return NULL;
  }

  // consume ";"
  parser->pos++;
  return stmt;
}

Parser *parse(char *source) {
  Lexer *lexer = lex(source);
  Parser *parser = new_parser(lexer);
  while (!parser_is_end(parser)) {
    Node *stmt = parser_stmt(parser);
    if (stmt != NULL) {
      parser_add_node(parser, stmt);
    }
  }

  return parser;
}

void parser_print_error(Parser *parser) {
  if (!parser_has_error(parser)) {
    return;
  }

  if (lex_has_error(parser->lexer)) {
    lex_print_error(parser->lexer);
  } else {
    // TODO: implement printing parser error
    Token *tok = parser_tok(parser);
    printf("Parser error: %s\n", parser->error);
    lex_print_excerpt(parser->lexer, tok->pos);
  }
}
