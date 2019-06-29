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
  
  int indent_str_len = indent * 2;
  char indent_str[indent_str_len + 1];
  for (int i = 0; i < indent_str_len; i++) {
    indent_str[i] = ' ';
  }
  indent_str[indent_str_len] = '\0';
  printf("%s", indent_str);

  switch (node->type) {
  case ND_IDENT:
    printf("ND_IDENT %s\n", node->ident);
    break;
  case ND_FN_CALL:
    printf("ND_FN_CALL %s\n", node->fn_call->name);
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
      printf("%s  .%s = \n", indent_str, key);
      print_node(expr, indent + 2);
    }
    break;
  case ND_ALLOC_ARRAY_EXPR:
    printf("ND_ALLOC_ARRAY_EXPR %s\n", node->alloc_array_expr->name);
    print_node(node->alloc_array_expr->size_expr, indent + 1);
    break;
  case ND_BINOP_EXPR:
    printf("ND_BINOP_EXPR ");
    switch (node->binop_expr->type) {
    case BO_EQ:
      printf("==\n");
      break;
    case BO_NEQ:
      printf("!=\n");
      break;
    case BO_LTE:
      printf("<\n");
      break;
    case BO_GTE:
      printf(">\n");
      break;
    case BO_LOGICAL_AND:
      printf("&&\n");
      break;
    case BO_LOGICAL_OR:
      printf("||\n");
      break;
    default:
      printf("%c\n", (char)node->binop_expr->type);
    }
    print_node(node->binop_expr->lhs, indent + 1);
    print_node(node->binop_expr->rhs, indent + 1);
  }
}

int main(int argc, char *argv[]) {

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
