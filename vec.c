#include "./vec.h"

Vec *new_vec() {
  Vec *vec = malloc(sizeof(Vec));
  vec->data = malloc(sizeof(void *) * 16);
  vec->capacity = 16;
  vec->len = 0;
  return vec;
}

void vec_push(Vec *vec, void *elem) {
  if (vec->capacity == vec->len) {
    vec->capacity *= 2;
    vec->data = realloc(vec->data, sizeof(void *) * vec->capacity);
  }

  vec->data[vec->len++] = elem;
}
