#include "lex.h"
#include "refcount.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  char *name;
  int age;
} Person;

Person *new_person(char *name, int age) {
  Person *p = (Person *)malloc(sizeof(Person));
  p->name = name;
  p->age = age;
  return p;
}

void person_destruct(void *p) {
  Person *person = (Person *)p;
  printf("destruct Person name: %s, age: %d\n", person->name, person->age);
  free(p);
}

int main(int argc, const char *argv[]) {
  Lexer *lexer = lex("type Person { int age; char tag; }");
  for (int i = 0; i < lexer->tok_len; i++) {
    Token *tok = lexer->tokens[i];
    switch (tok->type) {
    case TK_IDENT:
      printf("TK_IDENT %s\n", tok->val);
      break;
    default:
      printf("CH %d %c\n", tok->type, tok->type);
      break;
    }
  }
  return 0;
}
