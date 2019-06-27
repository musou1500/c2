#include <stdlib.h>
#include <stdio.h>

typedef struct Ref {
  void* value;
  int refcount;
  void (*destruct)(void *);
} Ref;

typedef struct Refs {
  Ref* val;
  struct Refs *next;
} Refs;

Refs* refs;

Refs* new_refs() {
  Refs* refs = (Refs *)malloc(sizeof(Refs));
  refs->next = NULL;
  refs->val = NULL;
  return refs;
}

void init_refs() {
  refs = new_refs();
}

Refs* refs_get_last(Refs* refs) {
  while(refs->next != NULL) {
    refs = refs->next;
  }

  return refs;
}

size_t refs_len() {
  size_t len = 0;
  while(refs->next != NULL) {
    refs = refs->next;
    len++;
  }

  return len;
}

void refs_add(Refs *refs, Ref *ref) {
  Refs* last = refs_get_last(refs);
  last->next = new_refs();
  last->next->val = ref;
}

void refs_destruct(Refs* ref) {
  while(refs->next != NULL) {
    Refs *next = refs->next;
    free(refs);
    refs = next;
  }
}

void ref_recycle(Ref* ref) {
  if (ref != NULL && ref->refcount == 0) {
    ref->destruct(ref->value);
  }
}

void ref_assign(Ref* lhs, Ref* rhs) {
  if(lhs != NULL) {
    lhs->refcount--;
  }

  if (rhs != NULL) {
    rhs->refcount++;
  }

  ref_recycle(lhs);
}

Ref* new_ref(void* value, void (*destruct)(void *)) {
  Ref* ref = (Ref *)malloc(sizeof(ref));
  refs_add(refs, ref);

  ref->value = value;
  ref->refcount = 0;
  ref->destruct = destruct;
  ref_assign(NULL, ref);
  return ref;
}


// Person definitions
typedef struct {
  char *name;
  int age;
} Person;


Person* new_person(char* name, int age) {
  Person* p = (Person *)malloc(sizeof(Person));
  p->name = name;
  p->age = age;
  return p;
}

void person_destruct(void *p) {
  Person* person = (Person*) p;
  printf("destruct Person name: %s, age: %d", person->name, person->age);
  free(p);
}


int main(int argc, const char *argv[])
{
  init_refs();

  // instantiate person objects
  Person* p1 = new_person("musou1500", 24);
  Ref* ref1 = new_ref(p1, person_destruct);
  
  Person* p2 = new_person("musou1501", 24);
  Ref* ref2 = new_ref(p1, person_destruct);

  ref_assign(ref2, ref1);
  return 0;
}
