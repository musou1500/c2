#include "./refcount.h"
#include <stdio.h>
#include <stdlib.h>

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

void ref_recycle(Ref *ref) {
  if (ref != NULL && ref->refcount == 0) {
    ref->destruct(ref->value);
  }
}

void ref_assign(Ref *lhs, Ref *rhs) {
  if (lhs != NULL) {
    lhs->refcount--;
  }

  if (rhs != NULL) {
    rhs->refcount++;
  }

  if (lhs != NULL) {
    ref_recycle(lhs);
  }

  if (lhs != NULL && rhs != NULL) {
    lhs->value = rhs->value;
  }
}
