#include "hashmap.h"
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct HashMap HashMap;
int enlarge_called = 0;

struct HashMap {
  Pair **buckets;
  long size;     // cantidad de datos/pairs en la tabla
  long capacity; // capacidad de la tabla
  long current;  // indice del ultimo dato accedido
};

Pair *createPair(char *key, void *value) {
  Pair *new = (Pair *)malloc(sizeof(Pair));
  new->key = key;
  new->value = value;
  return new;
}

long hash(char *key, long capacity) {
  unsigned long hash = 0;
  char *ptr;
  for (ptr = key; *ptr != '\0'; ptr++) {
    hash += hash * 32 + tolower(*ptr);
  }
  return hash % capacity;
}

int is_equal(void *key1, void *key2) {
  if (key1 == NULL || key2 == NULL)
    return 0;
  if (strcmp((char *)key1, (char *)key2) == 0)
    return 1;
  return 0;
}

void insertMap(HashMap *map, char *key, void *value) {
  if (map == NULL || key == NULL)
    return;
  if (map->size == map->capacity)
    return;
  long pos = hash(key, map->capacity);
  if (map->buckets[pos] != NULL) {
    while (map->buckets[pos] != NULL)
      pos = (pos + 1) % map->capacity;
    map->buckets[pos] = createPair(key, value);
  } else
    map->buckets[pos] = createPair(key, value);
  map->size++;
}

void enlarge(HashMap *map) {
  if (map == NULL || map->size == 0 || map->current == -1 || map->capacity == 0)
    return;
  enlarge_called = 1;
  map->size = 0;
  Pair **old_buckets = map->buckets;
  long newCapacity = map->capacity;
  map->capacity = newCapacity*2;
  map->buckets = (Pair **)calloc(map->capacity, sizeof(Pair *));
  for(long i=0;i<newCapacity;++i){
    if(old_buckets[i] != NULL)
      insertMap(map, old_buckets[i]->key, old_buckets[i]->value);
  }
}

HashMap *createMap(long capacity) {
  HashMap *map = (HashMap *)malloc(sizeof(HashMap));
  map->buckets = (Pair **)calloc(capacity, sizeof(Pair *));
  map->capacity = capacity;
  for (int i = 0; i < capacity; ++i)
    map->buckets[i] = NULL;
  map->size = 0;
  map->current = -1;
  return map;
}

void eraseMap(HashMap *map, char *key) {
  if (map == NULL || key == NULL)
    return;
  long pos = hash(key, map->capacity);
  if (is_equal(map->buckets[pos]->key, key)) {
    map->buckets[pos]->key = NULL;
    --map->size;
    return;
  }
  while (map->buckets[pos] != NULL) {
    if (is_equal(map->buckets[pos]->key, key)) {
      map->buckets[pos]->key = NULL;
      --map->size;
      return;
    }
    pos = (pos + 1) % map->capacity;
  }
}

Pair *searchMap(HashMap *map, char *key) {
  if (map == NULL || key == NULL)
    return NULL;
  long pos = hash(key, map->capacity);
  while (map->buckets[pos] != NULL) {
    if (is_equal(map->buckets[pos]->key, key)) {
      map->current = pos;
      return map->buckets[pos];
    }
    pos = (pos + 1) % map->capacity;
  }
  map->current = pos;
  return NULL;
}

Pair *firstMap(HashMap *map) {
  if (map == NULL)
    return NULL;
  for(long i=0;i<map->capacity;i++){
    if(map->buckets[i]!=NULL && map->buckets[i]->key!=NULL){
      map->current = i;
      return map->buckets[i];
    }
  }
  return NULL;
}

Pair *nextMap(HashMap *map) {
  if (map == NULL || map->size == 0 || map->current == -1 || map->capacity == 0)
    return NULL;
  if (map->current == map->capacity - 1)
    map->current = -1;
  else
    map->current++;
  while (map->buckets[map->current] == NULL || map->buckets[map->current]->key == NULL){
    if (map->current == map->capacity - 1){
      map->current = -1;
      return NULL;
    }
    map->current++;
  }
  return map->buckets[map->current];
}
