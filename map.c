#include "./map.h"
#include "./vec.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

Map *new_map() {
  Map *map = malloc(sizeof(Map));
  map->keys = new_vec();
  map->vals = new_vec();
  return map;
}

void map_put(Map *map, char *key, void *val) {
  vec_push(map->keys, key);
  vec_push(map->vals, val);
}

void *map_get(Map *map, char *key) {
  for (int i = map->keys->len - 1; i >= 0; i--)
    if (strcmp(map->keys->data[i], key) == 0)
      return map->vals->data[i];
  return NULL;
}

bool map_has(Map *map, char *key) { return map_get(map, key) != NULL; }
