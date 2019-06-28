#include "./lex.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Token *new_tok(TokTy type, char *val) {
  Token *tok = (Token *)malloc(sizeof(Token));
  tok->type = type;
  tok->val = val;
  return tok;
}

Lexer *new_lexer(char *source) {
  Lexer *lexer = (Lexer *)malloc(sizeof(Lexer));
  lexer->source = source;
  lexer->pos = 0;
  lexer->tok_len = 0;
  return lexer;
}

char lex_ch(Lexer *lexer) { return lexer->source[lexer->pos]; }

bool is_wschar(char ch) {
  switch (ch) {
  case ' ':
  case '\t':
  case '\r':
  case '\n':
    return true;
  default:
    return false;
  }
}

bool is_ident_char(char ch) {
  return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_';
}

bool lex_is_end(Lexer *lexer) { return strlen(lexer->source) <= lexer->pos; }

void lex_skipws(Lexer *lexer) {
  while (is_wschar(lex_ch(lexer)) && !lex_is_end(lexer)) {
    lexer->pos++;
  }
}

bool lex_forward_if(Lexer *lexer, char *name) {
  int orig_pos = lexer->pos;
  while (*name == lex_ch(lexer)) {
    lexer->pos++;
    name++;
  }

  if (*name == '\0') {
    return true;
  } else {
    lexer->pos = orig_pos;
    return false;
  }
}

void lex_add_tok(Lexer *lexer, int type) {
  lexer->tokens[lexer->tok_len++] = new_tok(type, NULL);
}

void lex_add_ident(Lexer *lexer, char *name) {
  lexer->tokens[lexer->tok_len++] = new_tok(TK_IDENT, name);
}

void lex_add_string(Lexer *lexer, char *str) {
  lexer->tokens[lexer->tok_len++] = new_tok(TK_STRING, str);
}

char lex_escape(Lexer *lexer) {
  // consume "\"
  lexer->pos++;

  char ch = lex_ch(lexer);
  switch(ch) {
    case '"':
    case '\\':
    case '/':
      lexer->pos++;
      return ch;
    case 'b': lexer->pos++; return '\b';
    case 'f': lexer->pos++; return '\f';
    case 'n': lexer->pos++; return '\n';
    case 'r': lexer->pos++; return '\r';
    case 't': lexer->pos++; return '\t';
    default:
      // error
      break;
  }
}

char* lex_chars(Lexer *lexer) {
  int len = 0;
  int orig_pos = lexer->pos;
  while(!lex_is_end(lexer)) {
    char ch = lex_ch(lexer);
    if (ch == '\\') {
      len++;
      lexer->pos += 2;
    } else if (ch != '\"') {
      len++;
      lexer->pos++;
    } else {
      break;
    }
  }

  lexer->pos = orig_pos;
  char *chars = (char *)malloc(sizeof(char) * len + 1);
  for (int i = 0; i < len; i++) {
    char ch = lex_ch(lexer);
    if (ch == '\\') {
      chars[i] = lex_escape(lexer);
    } else {
      chars[i] = ch;
      lexer->pos++;
    }
  }

  chars[len] = '\0';
  return chars;
}

char* lex_string(Lexer *lexer) {
  // consume "\""
  lexer->pos++;
  char *chars = lex_chars(lexer);
  char ch = lex_ch(lexer);
  if (ch == '\"') {
    lexer->pos++;
    return chars;
  }

  // error
}

Lexer *lex(char *source) {
  Lexer *lexer = new_lexer(source);
  while (!lex_is_end(lexer)) {
    lex_skipws(lexer);

    char ch = lex_ch(lexer);
    if (ch == '{' || ch == '}' || ch == '(' || ch == ')' || ch == '=' ||
        ch == 'n' || ch == ';' || ch == '.') {
      lex_add_tok(lexer, ch);
      lexer->pos++;
      continue;
    }

    if (ch == '"') {
      char *str = lex_string(lexer);
      lex_add_string(lexer, str);
      continue;
    }

    int len = 0;
    while (!is_wschar(lex_ch(lexer)) && is_ident_char(lex_ch(lexer))) {
      len++;
      lexer->pos++;
    }
    lexer->pos -= len;

    char *ident = (char *)malloc(sizeof(char) * len + 1);
    for (int i = 0; i < len; i++) {
      ident[i] = lex_ch(lexer);
      lexer->pos++;
    }

    ident[len] = '\0';
    lex_add_ident(lexer, ident);
  }

  return lexer;
}
