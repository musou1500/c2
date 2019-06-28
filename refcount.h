#ifndef _REFCOUNT_H
#define _REFCOUNT_H

#include <stdlib.h>

typedef struct {
  int count;
} RefCount;

typedef struct Ref {
  void* val;
  RefCount *count;
  void (*destruct)(void *);
} Ref;

typedef struct Refs {
  Ref* val;
  struct Refs *next;
} Refs;

extern Refs* refs;

// refcount
RefCount* new_ref_count(int count);
void ref_count_incr(RefCount *ref_count);
void ref_count_decr(RefCount *ref_count);


// refs
Refs* new_refs();
void init_refs();
Refs* refs_get_last();
size_t refs_len();
void refs_add(Ref *ref);
void refs_destruct();

// ref
Ref new_ref();
void ref_destruct(Ref *ref);
void ref_recycle(Ref* ref);
void ref_assign(Ref* l, Ref* r);
#endif
