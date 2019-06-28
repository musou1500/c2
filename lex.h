#ifndef _LEX_H
#define _LEX_H

#include <stdbool.h>
#define MAX_TOKENS (255)

typedef enum {
  TK_STRING = 256,
  TK_IDENT,
} TokTy;

typedef struct {
  int type;
  char *val;
} Token;

typedef struct {
  char *source;
  int pos;
  Token *tokens[MAX_TOKENS];
  int tok_len;
  char *error;
} Lexer;

Token *new_tok(TokTy type, char *val);
Lexer *new_lexer(char *source);
char lex_ch(Lexer *lexer);
bool is_wschar(char ch);
bool lex_is_end(Lexer* lexer);
void lex_skipws(Lexer *lexer);
void lex_add_tok(Lexer* lexer, int type);
void lex_add_ident(Lexer* lexer, char *name);
void lex_add_string(Lexer *lexer, char *str);
Lexer *lex(char *source);

#endif
