#include <stdlib.h>

typedef struct Ref {
  void* value;
  int refcount;
  void (*destruct)(void *);
} Ref;

typedef struct Refs {
  Ref* val;
  struct Refs *next;
} Refs;

extern Refs* refs;

// refs
Refs* new_refs();
void init_refs();
Refs* refs_get_last();
size_t refs_len();
void refs_add(Ref *ref);
void refs_destruct();

// ref
void ref_destruct(Ref *ref);
void ref_recycle(Ref* ref);
void ref_assign(Ref* l, Ref* r);
