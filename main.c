#include "parse.h"
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
  Parser *parser = parse("type Person; a = b;");
  if (parser->error != NULL) {
    printf("parser error: %s\n", parser->error);
  }

  Vec *nodes = parser->nodes;
  printf("%d nodes\n", nodes->len);
  for (int i = 0; i < nodes->len; i++) {
    Node *node = (Node *)nodes->data[i];

    switch (node->type) {
    case ND_FN_CALL:
      printf("fn call %s\n", node->fn_call->name);
      break;
    case ND_TYPE_DECL:
      printf("type decl %s\n", node->type_decl->name);
      break;
    case ND_VAR_DECL:
      printf("var decl %s = %s\n", node->var_decl->name, node->var_decl->ident);
      break;
    }
  }
  return 0;
}
