#include "./parse.h"
#include "./ast.h"
#include "./lex.h"
#include "./map.h"
#include "./vec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

Vec *parser_import_item_list(Parser *parser) {
  Vec *items = new_vec();

  if (parser_is_type(parser, '}')) {
    return items;
  }

  do {
    if (!parser_is_ident(parser)) {
      return NULL;
    }

    Token *ident = parser_tok(parser);
    vec_push(items, ident->val);
    parser->pos++;
  } while (parser_is_type(parser, ',') && parser->pos++);
  return items;
}

Node *parser_import(Parser *parser) {
  // consume "import"
  parser->pos++;

  if (!parser_is_type(parser, TK_STRING)) {
    parser_error(parser, "path expected after \"import\"");
    return NULL;
  }

  Token *path_tok = parser_tok(parser);
  parser->pos++;

  if (!parser_is_type(parser, '{')) {
    parser_error(parser, "\"{\" expected after path");
    return NULL;
  }

  parser->pos++;
  Vec *import_item_list = parser_import_item_list(parser);
  if (!parser_is_type(parser, '}')) {
    parser_error(parser, "\"}\" expected after import item list");
    return NULL;
  }
  parser->pos++;

  return new_import_node(path_tok->val, import_item_list);
}

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
Vec *parser_params(Parser *parser) {
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
  Vec *args = parser_params(parser);

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

TypeSpec *parser_type_spec(Parser *parser) {
  // parse array type spec
  if (parser_is_type(parser, '[')) {
    // consume ']'
    parser->pos++;
    TypeSpec *elem_type_spec = parser_type_spec(parser);
    if (parser_has_error(parser)) {
      return NULL;
    }

    if (!parser_is_type(parser, ']')) {
      parser_error(parser, "\"]\" is expected after array type specifier");
      return NULL;
    }

    // consume ']'
    parser->pos++;
    Vec *type_params = new_vec();
    vec_push(type_params, elem_type_spec);
    return new_type_spec("array", type_params);
  }

  if (parser_is_ident(parser)) {
    Token *tok = parser_tok(parser);
    parser->pos++;
    char *name = tok->val;
    Vec *type_params = new_vec();
    if (parser_is_type(parser, '<')) {
      parser->pos++;

      // TODO: implement parsing type params
      if (parser_is_type(parser, '>')) {
        parser_error(parser, "\">\" is expected after type params");
        return NULL;
      }
      parser->pos++;
    }

    return new_type_spec(name, type_params);
  }

  parser_error(parser, "unexpected token");
  return NULL;
}

Node *parser_var_decl(Parser *parser) {
  // consume "var"
  parser->pos++;
  Token *tk_ident = parser_tok(parser);
  // consume ident
  parser->pos++;

  TypeSpec *type_spec = NULL;
  if (parser_is_type(parser, ':')) {
    // consume ':'
    parser->pos++;
    type_spec = parser_type_spec(parser);
    if (parser_has_error(parser)) {
      return NULL;
    }
  }

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

  Node *node = new_var_decl_node(tk_ident->val, expr);
  node->type_spec = type_spec;
  return node;
}

Map *parser_inits(Parser *parser) {
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

  Map *inits = parser_inits(parser);
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
  if (parser_is_ident_of(parser, "new")) {
    return parser_alloc_expr(parser);
  }

  // function call or identifier
  if (parser_is_ident(parser)) {
    parser->pos++;
    Token *next_tok = parser_tok(parser);
    parser->pos--;
    if (tok_is_type(next_tok, '(')) {
      return parser_fn_call(parser);
    }

    // consume ident
    parser->pos++;
    return new_ident_node(tok->val);
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

Node *parser_index(Parser *parser) {
  Node *lhs = parser_term(parser);
  if (parser_is_type(parser, '[')) {
    parser->pos++;
    Node *rhs = parser_expr(parser);

    if (!parser_is_type(parser, ']')) {
      parser_error(parser, "\"]\" is expected");
      return NULL;
    }

    parser->pos++;
    return new_binop_expr_node(BO_INDEX, lhs, rhs);
  }

  return lhs;
}

Node *parser_mult_div(Parser *parser) {
  Node *lhs = parser_index(parser);
  if (parser_has_error(parser)) {
    return NULL;
  }

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
  if (parser_has_error(parser)) {
    return NULL;
  }

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
  if (parser_has_error(parser)) {
    return NULL;
  }

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
  if (parser_has_error(parser)) {
    return NULL;
  }

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
  if (parser_has_error(parser)) {
    return NULL;
  }

  if (parser_is_type(parser, TK_LOGICAL_OR)) {
    parser->pos++;
    return new_binop_expr_node(BO_LOGICAL_OR, lhs, parser_logical_and(parser));
  }

  return lhs;
}

Node *parser_stmt(Parser *parser);

Vec *parser_block_stmt(Parser *parser) {
  // consume '{'
  parser->pos++;
  Vec *stmts = new_vec();
  while (!parser_is_end(parser) && !parser_is_type(parser, '}')) {
    Node *stmt = parser_stmt(parser);
    if (parser_has_error(parser)) {
      return NULL;
    }

    vec_push(stmts, stmt);
  }

  if (!parser_is_type(parser, '}')) {
    parser_error(parser, "\"}\" is expected after block stmt");
    return NULL;
  }

  parser->pos++;
  return stmts;
}

Node *parser_elif(Parser *parser) { return NULL; }

Node *parser_if_stmt(Parser *parser) {
  Node *cond = NULL;
  if (parser_is_ident_of(parser, "if")) {
    // consume "if"
    parser->pos++;
    cond = parser_expr(parser);
    if (parser_has_error(parser)) {
      return NULL;
    }
  }

  if (!parser_is_type(parser, '{')) {
    parser_error(parser, "\"{\" is expected if condition");
    return NULL;
  }

  // consume '{'
  Vec *stmts = parser_block_stmt(parser);
  if (parser_has_error(parser)) {
    return NULL;
  }

  if (parser_is_ident_of(parser, "else")) {
    parser->pos++;
    return new_if_stmt_node(cond, stmts, parser_if_stmt(parser));
  } else {
    return new_if_stmt_node(cond, stmts, NULL);
  }
}

Node *parser_ret_stmt(Parser *parser) {
  // consume "return"
  parser->pos++;
  Node *expr = parser_expr(parser);
  if (parser_has_error(parser)) {
    return NULL;
  }

  return new_ret_stmt_node(expr);
}

Node *parser_fn_decl(Parser *parser);
Node *parser_stmt(Parser *parser) {
  Node *stmt;
  if (parser_is_ident_of(parser, "if")) {
    return parser_if_stmt(parser);
  } else if (parser_is_ident_of(parser, "fn")) {
    return parser_fn_decl(parser);
  }

  if (parser_is_ident_of(parser, "var")) {
    stmt = parser_var_decl(parser);
  } else if (parser_is_ident_of(parser, "type")) {
    stmt = parser_type_decl(parser);
  } else if (parser_is_ident_of(parser, "return")) {
    stmt = parser_ret_stmt(parser);
  } else if (parser_is_ident_of(parser, "import")) {
    stmt = parser_import(parser);
  } else {
    stmt = parser_expr(parser);
  }

  if (parser_has_error(parser)) {
    return NULL;
  }

  if (!parser_is_type(parser, ';')) {
    parser_error(parser, "\";\" is expected after statement");
    return NULL;
  }

  // consume ";"
  parser->pos++;
  return stmt;
}

Vec *parser_args(Parser *parser) {
  // consume '('
  parser->pos++;

  Vec *args = new_vec();
  if (parser_is_type(parser, ')')) {
    parser->pos++;
    return args;
  }

  do {
    if (!parser_is_ident(parser)) {
      parser_error(parser, "identifier expected");
      return NULL;
    }

    Token *tok_ident = parser_tok(parser);
    parser->pos++;

    Node *arg = new_ident_node(tok_ident->val);
    if (!parser_is_type(parser, ':')) {
      parser_error(parser, "arg type can not be omitted");
      return NULL;
    }

    parser->pos++;
    TypeSpec *type_spec = parser_type_spec(parser);
    arg->type_spec = type_spec;

    vec_push(args, arg);
  } while (parser_is_type(parser, ',') && parser->pos++);

  if (!parser_is_type(parser, ')')) {
    parser_error(parser, "\")\" expected after args");
    return NULL;
  }

  parser->pos++;
  return args;
}

Node *parser_fn_decl(Parser *parser) {
  // consume "fn"
  parser->pos++;
  if (!parser_is_ident(parser)) {
    parser_error(parser, "identifier expected after \"fn\"");
    return NULL;
  }

  Token *tok_ident = parser_tok(parser);
  parser->pos++;

  if (!parser_is_type(parser, '(')) {
    parser_error(parser, "\"(\" expected after function name");
    return NULL;
  }

  Vec *args = parser_args(parser);
  if (parser_has_error(parser)) {
    return NULL;
  }

  TypeSpec *ret_type_spec = NULL;
  if (!parser_is_type(parser, ':')) {
    parser_error(parser, "return type can not be omitted");
    return NULL;
  }

  parser->pos++;
  ret_type_spec = parser_type_spec(parser);

  if (!parser_is_type(parser, '{')) {
    parser_error(parser, "\"{\" is expected before statements");
    return NULL;
  }

  Vec *stmts = parser_block_stmt(parser);
  if (parser_has_error(parser)) {
    return NULL;
  }

  return new_fn_decl_node(tok_ident->val, args, ret_type_spec, stmts);
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
