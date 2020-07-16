#ifndef __HASH_MAP_H__
#define __HASH_MAP_H__

#include "list.h"
#include "scope.h"

typedef struct 
{
    long key;
    long index;
} KeyIndex;

typedef List(KeyIndex) ListKeyIndex;
typedef List(ListKeyIndex) ListListKeyIndex;

#define _HashMap(V) struct { ListListKeyIndex _table; ListLong keys; List(V) values; Scope scope; }

typedef _HashMap(long) HashMap;

HashMap newHashMap(Scope scope, int bucketSize);
int hashMapSize(HashMap m);
bool hashMapContainsKey(HashMap m, long key);
void hashMapPut(HashMap m, long key, long value);
long hashMapGet(HashMap m, long key);
long hashMapGetOrDefault(HashMap m, long key, long defaultValue);

#endif