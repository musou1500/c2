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

Refs *refs_get_last(Refs *refs) {
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

void refs_add(Refs *refs, Ref *ref) {
  Refs *last = refs_get_last(refs);
  last->next = new_refs();
  last->next->val = ref;
}

void refs_destruct(Refs *ref) {
  while (refs->next != NULL) {
    Refs *next = refs->next;
    free(refs);
    refs = next;
  }
}

void ref_destruct(Ref *ref) {
  printf("destruct Ref tag: %s\n", ref->tag);
  free(ref);
}

void ref_recycle(Ref *ref) {
  if (ref != NULL && ref->refcount == 0) {
    ref->destruct(ref->value);
  }
}

void ref_assign(Ref **l, Ref **r) {
  if (l != NULL) {
    (*l)->refcount--;
  }

  if (r != NULL) {
    (*r)->refcount++;
  }

  if (l != NULL) {
    ref_recycle(*l);
  }

  if (l != NULL && (*l)->refcount == 0 && r == NULL) {
    ref_destruct(*l);
  }

  if (l != NULL && r != NULL) {
    ref_destruct(*l);
    *l = *r;
  }
}

Ref *new_ref(void *value, char *tag, void (*destruct)(void *)) {
  Ref *ref = (Ref *)malloc(sizeof(ref));
  refs_add(refs, ref);

  ref->value = value;
  ref->refcount = 0;
  ref->destruct = destruct;
  ref->tag = tag;
  ref_assign(NULL, &ref);
  return ref;
}
