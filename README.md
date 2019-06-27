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

## refcount language

```
extern type Person;
// p
// GCObject(Person)
//   val
//     name: "musou1500"
//   refcount: 1
// p2
// GCObject(Person)
//   val
//     name: "musou1501"
//   refcount: 1
p = Person("musou1500");
p2 = Person("musou1501");

// GCObject.assign
p = p2
```
