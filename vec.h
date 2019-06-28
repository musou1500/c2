#ifndef _VEC_H
#define _VEC_H


typedef struct {
  void **data;
  int capacity;
  int len;
} Vec;
Vec *new_vec();
void vec_push(Vec *vec, void* elem);
#endif
