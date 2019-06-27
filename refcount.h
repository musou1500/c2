#include <stdlib.h>

typedef struct Ref {
  void* value;
  char* tag;
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
Refs* refs_get_last(Refs* refs);
size_t refs_len();
void refs_add(Refs *refs, Ref *ref);
void refs_destruct(Refs* ref);

// ref
Ref* new_ref(void* value, char* tag, void (*destruct)(void *));
void ref_destruct(Ref *ref);
void ref_recycle(Ref* ref);
void ref_assign(Ref** l, Ref** r);
