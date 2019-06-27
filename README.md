# learn_gc

GCを自分で実装して仕組みを理解する．

## refcount
参照カウント方式．

```c
Person* p1 = new_person("musou1500", 24);
Ref* ref1 = new_ref(p1, person_destruct);

Person* p2 = new_person("musou1501", 24);
Ref* ref2 = new_ref(p1, person_destruct);

// ここで p2 は開放される
ref_assign(ref2, ref1);
```
