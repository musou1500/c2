#ifndef _LEX_H
#define _LEX_H

#include <stdbool.h>
#include "./vec.h"
#define MAX_TOKENS (255)

typedef enum {
  TK_STRING = 256,
  TK_IDENT,
  TK_NUMBER,
  TK_EOF
} TokTy;

typedef struct {
  int type;
  union {
    char *val;
    double n_val;
  };
} Token;

typedef struct {
  char *source;
  int pos;
  Vec *tokens;
  char *error;
} Lexer;

Token *new_tok(TokTy type, char *val);
Token *new_tok_number(TokTy type, double val);
Lexer *new_lexer(char *source);
char lex_ch(Lexer *lexer);
bool is_wschar(char ch);
bool lex_is_end(Lexer* lexer);
void lex_skipws(Lexer *lexer);
void lex_add_tok(Lexer* lexer, int type);
void lex_add_ident(Lexer* lexer, char *name);
void lex_add_string(Lexer *lexer, char *str);
void lex_add_number(Lexer *lexer, double number);
int lex_int(Lexer *lexer);
char *lex_digits(Lexer *lexer);
char lex_digit(Lexer *lexer);
double lex_frac(Lexer *lexer);
double lex_number(Lexer *lexer);
Lexer *lex(char *source);

#endif
