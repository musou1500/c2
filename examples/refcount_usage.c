#include "../refcount.h"

int main(int argc, const char *argv[])
{
  printf("gc:")
  Person* p1 = new_person("musou1500", 24);
  Ref ref1 = new_ref(p1, person_destruct);

  Person* p2 = new_person("musou1501", 24);
  Ref ref2 = new_ref(p2, person_destruct);

  printf("ref2 = ref1;\n");
  ref_assign(&ref2, &ref1);

  printf("ref1 = NULL;\n");
  ref_assign(&ref1, NULL);

  printf("ref2 = NULL;\n");
  ref_assign(&ref1, NULL);
  printf("\n\n"); 
  return 0;
}
