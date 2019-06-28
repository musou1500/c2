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
  init_refs();

  Person* p1 = new_person("musou1500", 24);
  Ref ref1 = new_ref(p1, person_destruct);
  refs_add(&ref1);

  Person* p2 = new_person("musou1501", 24);
  Ref ref2 = new_ref(p2, person_destruct);
  refs_add(&ref2);
  
  printf("ref2 = ref1;\n");
  ref_assign(&ref2, &ref1);
  
  printf("ref1 = NULL;\n");
  ref_assign(&ref1, NULL);

  printf("ref2 = NULL;\n");
  ref_assign(&ref1, NULL);

  Lexer *lexer = lex("type Person; b = \"str\";");
  for (int i = 0; i < lexer->tok_len; i++) {
    Token *tok = lexer->tokens[i];
    switch (tok->type) {
      case TK_IDENT:
        printf("TK_IDENT %s\n", tok->val);
        break;
      case TK_STRING:
        printf("TK_STRING %s\n", tok->val);
        break;
      default:
        printf("CH %c\n", tok->type);
        break;
    }
  }

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
