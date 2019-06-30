#ifndef _PARSE_H
#define _PARSE_H

#include "./lex.h"
#include "./ast.h"
#include "./vec.h"
#include "./map.h"

typedef struct {
  Lexer *lexer;
  Vec *nodes;
  int pos;
  char *error;
} Parser;

Parser *parse(char *source);
bool parser_has_error(Parser *parser);
void parser_print_error(Parser *parser);

#endif
