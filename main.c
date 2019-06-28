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

void print_node(Node *node, int ident) {
  for (int i = 0; i < ident; i++) {
    printf("  ");
  }

  switch (node->type) {
    case ND_FN_CALL:
      printf("fn call %s\n", node->fn_call->name);
      Vec *args = node->fn_call->args;
      for (int i = 0; i < args->len; i++) {
        Node *arg = (Node *)args->data[i];
        print_node(arg, ident + 1);
      }
      break;
    case ND_TYPE_DECL:
      printf("ND_TYPE_DECL %s\n", node->type_decl->name);
      break;
    case ND_VAR_DECL:
      printf("ND_VAR_DECL %s = \n", node->var_decl->name);
      print_node(node->var_decl->expr, ident + 1);
      break;
    case ND_STRING_LIT:
      printf("ND_STRING_LIT %s\n", node->str_lit);
      break;
    case ND_NUMBER_LIT:
      printf("ND_NUMBER_LIT %f\n", node->num_lit);
      break;
  }
}

int main(int argc, const char *argv[]) {
  /* printf("gc:") */
  /* Person* p1 = new_person("musou1500", 24); */
  /* Ref ref1 = new_ref(p1, person_destruct); */

  /* Person* p2 = new_person("musou1501", 24); */
  /* Ref ref2 = new_ref(p2, person_destruct); */

  /* printf("ref2 = ref1;\n"); */
  /* ref_assign(&ref2, &ref1); */

  /* printf("ref1 = NULL;\n"); */
  /* ref_assign(&ref1, NULL); */

  /* printf("ref2 = NULL;\n"); */
  /* ref_assign(&ref1, NULL); */
  /* printf("\n\n"); */
  char *source = "type Person;\np = Person(\"musou1500\", 24.0);";
  printf("source:\n%s\n\n", source);
  Lexer *lexer = lex(source);
  if (lexer->error != NULL) {
    printf("lexer error: %s\n", lexer->error);
    return 1;
  }

  printf("tokens: ");
  for (int i = 0; i < lexer->tokens->len; i++) {
    Token *tok = (Token *)lexer->tokens->data[i];
    switch (tok->type) {
      case TK_IDENT:
        printf("TK_IDENT %s, ", tok->val);
        break;
      case TK_STRING:
        printf("TK_STRING %s, ", tok->val);
        break;
      case TK_NUMBER:
        printf("TK_NUMBER %f, ", tok->n_val);
        break;
      default:
        printf("CH \"%c\", ", tok->type);
        break;
    }
  }
  printf("\n\n");
  
  Parser *parser = parse(source);
  if (parser->error != NULL) {
    printf("parser error: %d %s\n", parser->pos, parser->error);
    return 1;
  }

  Vec *nodes = parser->nodes;
  printf("ast:\n");
  for (int i = 0; i < nodes->len; i++) {
    Node *node = (Node *)nodes->data[i];
    print_node(node, 0);
  }
  return 0;
}
