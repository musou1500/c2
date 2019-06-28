#include "./refcount.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

RefCount *new_ref_count(int count) {
  RefCount *ref_count = (RefCount *)malloc(sizeof(RefCount));
  ref_count->count = count;
  return ref_count;
}

void ref_count_decr(RefCount *ref_count) { ref_count->count--; }

void ref_count_incr(RefCount *ref_count) { ref_count->count++; }

Ref new_ref(void *val, void (*destruct)(void *)) {
  Ref ref = {.val = val, .count = new_ref_count(1), .destruct = destruct};

  return ref;
}

bool ref_has_no_ref(Ref *ref) { return ref->count->count == 0; }

void ref_recycle(Ref *ref) {
  if (ref != NULL && ref_has_no_ref(ref)) {
    if (ref->destruct != NULL) {
      ref->destruct(ref->val);
    } else {
      free(ref->val);
    }

    free(ref->count);
  }
}

void ref_assign(Ref *lhs, Ref *rhs) {
  ref_count_decr(lhs->count);

  if (rhs != NULL) {
    ref_count_incr(rhs->count);
  }

  ref_recycle(lhs);

  if (rhs != NULL) {
    lhs->val = rhs->val;
    lhs->count = rhs->count;
    lhs->destruct = rhs->destruct;
  }
}
