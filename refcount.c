#include "./refcount.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

Refs *refs;
Refs *new_refs() {
  Refs *refs = (Refs *)malloc(sizeof(Refs));
  refs->next = NULL;
  refs->val = NULL;
  return refs;
}

void init_refs() { refs = new_refs(); }

Refs *refs_get_last() {
  while (refs->next != NULL) {
    refs = refs->next;
  }

  return refs;
}

size_t refs_len() {
  size_t len = 0;
  while (refs->next != NULL) {
    refs = refs->next;
    len++;
  }

  return len;
}

void refs_add(Ref *ref) {
  Refs *last = refs_get_last(refs);
  last->next = new_refs();
  last->next->val = ref;
}

void refs_destruct() {
  while (refs->next != NULL) {
    Refs *next = refs->next;
    free(refs);
    refs = next;
  }
}

RefCount* new_ref_count(int count) {
  RefCount* ref_count = (RefCount *)malloc(sizeof(RefCount));
  ref_count->count = count;
  return ref_count;
}

void ref_count_decr(RefCount* ref_count) {
  ref_count->count--;
}

void ref_count_incr(RefCount* ref_count) {
  ref_count->count++;
}

Ref new_ref(void* val, void (*destruct)(void *)) {
  Ref ref = {
    .val = val,
    .count = new_ref_count(1),
    .destruct = destruct
  };

  return ref;
}

bool ref_has_no_ref(Ref *ref) {
  return ref->count->count == 0;
}

void ref_recycle(Ref *ref) {
  if (ref != NULL && ref_has_no_ref(ref)) {
    ref->destruct(ref->val);
  }
}

void ref_assign(Ref *lhs, Ref *rhs) {
  ref_count_decr(lhs->count);

  if (rhs != NULL) {
    ref_count_incr(rhs->count);
  }

  ref_recycle(lhs);

  if(rhs != NULL) {
    lhs->val = rhs->val;
    lhs->count = rhs->count;
    lhs->destruct = rhs->destruct;
  }
}
