#ifndef __HASH_MAP_H__
#define __HASH_MAP_H__

#include "list.h"
#include "object.h"

typedef struct 
{
    long key;
    long value;
} HashMapItemL2L;

typedef List(HashMapItemL2L) ListHashMapItemL2L;

typedef struct
{
    List(ListHashMapItemL2L) _table;
    int _size;                                                                                                  \
    Scope scope;
}_HashMapL2LData, *HashMapL2L;

HashMapL2L newHashMapL2L(Scope scope, int bucketSize);
int hashMapL2LSize(HashMapL2L m);
bool hashMapL2LContainsKey(HashMapL2L m, long key);
void hashMapL2LPut(HashMapL2L m, long key, long value);
long hashMapL2LGet(HashMapL2L m, long key, long defaultValue);
ListLong hashMapL2LKeys(HashMapL2L m);
ListLong hashMapL2LValues(HashMapL2L m);
ListHashMapItemL2L hashMapL2LItems(HashMapL2L m) ;

#endif