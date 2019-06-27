#include <stdbool.h>
#define MAX_TOKENS (255)

typedef enum {
  TK_IDENT = 256,
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
} Lexer;

Token *new_tok(TokTy type, char *val);
Lexer *new_lexer(char *source);
char lex_ch(Lexer *lexer);
bool is_wschar(char ch);
bool lex_is_end(Lexer* lexer);
void lex_skipws(Lexer *lexer);
bool lex_forward_if(Lexer* lexer, char *name);
void lex_add_tok(Lexer* lexer, int type);
void lex_add_ident(Lexer* lexer, char *name);
Lexer *lex(char *source);
