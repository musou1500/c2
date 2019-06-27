#include "refcount.h"
#include <stdlib.h>
#include <stdio.h>

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
  printf("destruct Person name: %s, age: %d\n", person->name, person->age);
  free(p);
}


int main(int argc, const char *argv[])
{
  init_refs();

  // instantiate person objects
  Person* p1 = new_person("musou1500", 24);
  Ref* ref1 = new_ref(p1, "ref1", person_destruct);
  
  Person* p2 = new_person("musou1501", 24);
  Ref* ref2 = new_ref(p2, "ref2", person_destruct);
  // ref1(rc 1) -> musou1500
  // ref2(rc 1) -> musou1501

  ref_assign(&ref2, &ref1);
  // ref1(rc 2) ----> musou1500
  // ref2(rc 0) _/

  ref_assign(&ref1, NULL);
  // ref1(rc 1)    NULL
  // ref2(rc 0) -> musou1500

  ref_assign(&ref2, NULL);
  // ref1(rc 0)    NULL
  // ref2(rc 0)    NULL
  return 0;
}
