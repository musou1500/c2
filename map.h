#ifndef _MAP_H
#define _MAP_H

#include "./vec.h"
#include <stdbool.h>

typedef struct {
  Vec *keys;
  Vec *vals;
} Map;

Map *new_map();
void map_put(Map *map, char *key, void *val);
void *map_get(Map *map, char *key);
bool map_has(Map *map, char* key);

#endif
