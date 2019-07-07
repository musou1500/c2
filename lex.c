#include "./lex.h"
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Token *new_tok(TokTy type, char *val, int pos) {
  Token *tok = (Token *)malloc(sizeof(Token));
  tok->type = type;
  tok->val = val;
  tok->pos = pos;
  return tok;
}

Token *new_tok_number(TokTy type, double val, int pos) {
  Token *tok = (Token *)malloc(sizeof(Token));
  tok->type = type;
  tok->n_val = val;
  tok->pos = pos;
  return tok;
}

Lexer *new_lexer(char *source) {
  Lexer *lexer = (Lexer *)malloc(sizeof(Lexer));
  lexer->source = source;
  lexer->pos = 0;
  lexer->tokens = new_vec();
  lexer->error = NULL;
  return lexer;
}

char lex_ch(Lexer *lexer) { return lexer->source[lexer->pos]; }

void lex_error(Lexer *lexer, char *message) { lexer->error = message; }

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

bool lex_has_error(Lexer *lexer) { return lexer->error != NULL; }

bool lex_is_end(Lexer *lexer) {
  return strlen(lexer->source) <= lexer->pos || lex_has_error(lexer);
}

void lex_skipws(Lexer *lexer) {
  while (is_wschar(lex_ch(lexer)) && !lex_is_end(lexer)) {
    lexer->pos++;
  }
}

void lex_add_tok(Lexer *lexer, int type) {
  vec_push(lexer->tokens, new_tok(type, NULL, lexer->pos));
}

void lex_add_ident(Lexer *lexer, char *name) {
  vec_push(lexer->tokens, new_tok(TK_IDENT, name, lexer->pos));
}

void lex_add_string(Lexer *lexer, char *str) {
  vec_push(lexer->tokens, new_tok(TK_STRING, str, lexer->pos));
}

void lex_add_number(Lexer *lexer, double number) {
  vec_push(lexer->tokens, new_tok_number(TK_NUMBER, number, lexer->pos));
}

char lex_escape(Lexer *lexer) {
  // consume "\"
  lexer->pos++;

  char ch = lex_ch(lexer);
  switch (ch) {
  case '"':
  case '\\':
  case '/':
    lexer->pos++;
    return ch;
  case 'b':
    lexer->pos++;
    return '\b';
  case 'f':
    lexer->pos++;
    return '\f';
  case 'n':
    lexer->pos++;
    return '\n';
  case 'r':
    lexer->pos++;
    return '\r';
  case 't':
    lexer->pos++;
    return '\t';
  default:
    lex_error(lexer, "unexpected token");
    return '\0';
  }
}

char *lex_chars(Lexer *lexer) {
  int len = 0;
  int orig_pos = lexer->pos;
  while (!lex_is_end(lexer)) {
    char ch = lex_ch(lexer);

    // shoule close double quote before newline
    if (ch == '\n') {
      break;
    }

    // end of string literal
    if (ch == '\"') {
      break;
    }

    // escape sequence or normal char
    if (ch == '\\') {
      len++;
      lexer->pos += 2;
    } else {
      len++;
      lexer->pos++;
    }
  }

  lexer->pos = orig_pos;
  char *chars = (char *)malloc(sizeof(char) * len + 1);
  for (int i = 0; i < len && !lex_is_end(lexer); i++) {
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

int lex_int(Lexer *lexer) {
  char is_negative = lex_ch(lexer) == '-';
  if (is_negative) {
    lexer->pos++;
  }

  char fst_digit = lex_digit(lexer);
  if (lex_has_error(lexer)) {
    return -1;
  }

  if (fst_digit == '0') {
    return 0;
  }

  // convert fst_digit + digits to int
  char *digits = lex_digits(lexer);
  char digits_len = strlen(digits);
  int num = atoi(digits);
  num += (fst_digit - '0') * pow(10.0, digits_len);
  return is_negative ? -num : num;
}

char *lex_digits(Lexer *lexer) {
  int len = 0;
  while (!lex_is_end(lexer)) {
    if (isdigit(lex_ch(lexer))) {
      lexer->pos++;
      len++;
    } else {
      break;
    }
  }

  char *num = (char *)malloc(sizeof(char) * len + 1);
  lexer->pos -= len;
  for (int i = 0; i < len; i++) {
    num[i] = lex_ch(lexer);
    lexer->pos++;
  }
  num[len] = '\0';
  return num;
}

char lex_digit(Lexer *lexer) {
  char ch = lex_ch(lexer);
  if (isdigit(ch)) {
    lexer->pos++;
    return ch;
  }

  lex_error(lexer, "0-9 is expected");
  return '\0';
}

double lex_frac(Lexer *lexer) {
  if (lex_ch(lexer) == '.') {
    lexer->pos++;
    char *digits = lex_digits(lexer);
    int exp = -strlen(digits);
    return atoi(digits) * pow(10, exp);
  } else {
    return 0;
  }
}

// TODO: return Number instead of double
double lex_number(Lexer *lexer) {
  int int_part = lex_int(lexer);
  double frac_part = lex_frac(lexer);
  return int_part + frac_part;
}

char *lex_string(Lexer *lexer) {
  // consume "\""
  lexer->pos++;
  char *chars = lex_chars(lexer);
  if (lex_has_error(lexer)) {
    return NULL;
  }

  char ch = lex_ch(lexer);
  if (ch == '\"') {
    lexer->pos++;
    return chars;
  }

  lex_error(lexer, "\" is expected at end of string literal");
  return NULL;
}

Lexer *lex(char *source) {
  Lexer *lexer = new_lexer(source);
  while (true) {
    lex_skipws(lexer);
    if (lex_is_end(lexer)) {
      break;
    }

    char ch = lex_ch(lexer);
    if (ch == '{' || ch == '}' || ch == '(' || ch == ')' || ch == ';' ||
        ch == '.' || ch == ',' || ch == '[' || ch == ']' || ch == '+' ||
        ch == '-' || ch == '/' || ch == '*' || ch == ':') {
      lex_add_tok(lexer, ch);
      lexer->pos++;
      continue;
    }

    if (ch == '!') {
      lexer->pos++;
      char next_ch = lex_ch(lexer);
      if (next_ch == '=') {
        lexer->pos++;
        lex_add_tok(lexer, TK_NEQ);
      } else {
        lex_add_tok(lexer, '!');
      }

      continue;
    }

    if (ch == '<') {
      lexer->pos++;
      char next_ch = lex_ch(lexer);
      if (next_ch == '=') {
        lexer->pos++;
        lex_add_tok(lexer, TK_LTE);
      } else {
        lex_add_tok(lexer, '<');
      }

      continue;
    }

    if (ch == '>') {
      lexer->pos++;
      char next_ch = lex_ch(lexer);
      if (next_ch == '=') {
        lexer->pos++;
        lex_add_tok(lexer, TK_GTE);
      } else {
        lex_add_tok(lexer, '>');
      }

      continue;
    }

    if (ch == '=') {
      lexer->pos++;
      char next_ch = lex_ch(lexer);
      if (ch == next_ch) {
        lexer->pos++;
        lex_add_tok(lexer, TK_EQ);
      } else {
        lex_add_tok(lexer, '=');
      }

      continue;
    }

    if (ch == '&' || ch == '|') {
      lexer->pos++;
      char next_ch = lex_ch(lexer);
      if (ch == next_ch) {
        lexer->pos++;
        lex_add_tok(lexer, ch == '&' ? TK_LOGICAL_AND : TK_LOGICAL_OR);
      } else {
        if (ch == '&') {
          lex_error(lexer, "& is expected after &");
        } else {
          lex_error(lexer, "| is expected after |");
        }
      }
      continue;
    }

    if (ch == '"') {
      char *str = lex_string(lexer);
      if (!lex_has_error(lexer)) {
        lex_add_string(lexer, str);
      }
      continue;
    }

    if (ch == '-' || isdigit(ch)) {
      // parse number
      double number = lex_number(lexer);
      lex_add_number(lexer, number);
      continue;
    }

    if (is_ident_char(ch)) {
      int len = 0;
      char ident_ch = lex_ch(lexer);
      while (!is_wschar(ident_ch) &&
             (is_ident_char(ident_ch) || isdigit(ident_ch))) {
        len++;
        lexer->pos++;
        ident_ch = lex_ch(lexer);
      }
      lexer->pos -= len;

      char *ident = (char *)malloc(sizeof(char) * len + 1);
      for (int i = 0; i < len; i++) {
        ident[i] = lex_ch(lexer);
        lexer->pos++;
      }

      ident[len] = '\0';
      lex_add_ident(lexer, ident);
      continue;
    }

    lex_error(lexer, "unexpected char");
  }

  lex_add_tok(lexer, TK_EOF);

  return lexer;
}

void lex_print_excerpt(Lexer *lexer, int pos) {
  // find nearest newline
  int start_pos = pos;
  do {
    start_pos--;
  } while (start_pos > 0 && lexer->source[start_pos] != '\n');

  int end_pos = pos;
  while (end_pos < strlen(lexer->source) && lexer->source[end_pos] != '\n') {
    end_pos++;
  }

  // create excerpt
  int excerpt_len = end_pos - start_pos + 1;
  char excerpt[excerpt_len];
  for (int i = 0; i < excerpt_len - 1; i++) {
    excerpt[i] = lexer->source[start_pos + i];
  }
  excerpt[excerpt_len - 1] = '\0';

  printf("%s\n", excerpt);
  int offset = pos - start_pos;
  for (int i = 0; i < offset; i++) {
    printf(" ");
  }
  printf("^ error!\n");
}

void lex_print_error(Lexer *lexer) {
  if (!lex_has_error(lexer)) {
    return;
  }

  printf("Lexer error: %s\n", lexer->error);
  lex_print_excerpt(lexer, lexer->pos);
}
