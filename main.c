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

void print_node(Node *node, int indent) {
  for (int i = 0; i < indent; i++) {
    printf("  ");
  }

  switch (node->type) {
  case ND_FN_CALL:
    printf("fn call %s\n", node->fn_call->name);
    Vec *args = node->fn_call->args;
    for (int i = 0; i < args->len; i++) {
      Node *arg = (Node *)args->data[i];
      print_node(arg, indent + 1);
    }
    break;
  case ND_TYPE_DECL:
    printf("ND_TYPE_DECL %s\n", node->type_decl->name);
    break;
  case ND_VAR_DECL:
    printf("ND_VAR_DECL %s = \n", node->var_decl->name);
    print_node(node->var_decl->expr, indent + 1);
    break;
  case ND_STRING_LIT:
    printf("ND_STRING_LIT %s\n", node->str_lit);
    break;
  case ND_NUMBER_LIT:
    printf("ND_NUMBER_LIT %f\n", node->num_lit);
    break;
  case ND_ALLOC_EXPR:
    printf("ND_ALLOC_EXPR %s\n", node->alloc_expr->name);
    Map *inits = node->alloc_expr->inits;
    for (int i = 0; i < inits->keys->len; i++) {
      char *key = (char *)inits->keys->data[i];
      Node *expr = (Node *)map_get(inits, key);
      printf("  .%s =\n", key);
      print_node(expr, indent + 2);
    }
    break;
  case ND_ALLOC_ARRAY_EXPR:
    printf("ND_ALLOC_ARRAY_EXPR %s\n", node->alloc_array_expr->name);
    print_node(node->alloc_array_expr->size_expr, indent + 1);
    break;
  }
}

int main(int argc, char *argv[]) {
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
  char *source = argv[1];
  printf("source:\n%s\n\n", source);

  Parser *parser = parse(source);
  if (parser_has_error(parser)) {
    parser_print_error(parser);
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
