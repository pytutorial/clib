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

ListListKeyIndex newIndexTable(Scope scope, int bucketSize) ;
int indexTableGetIndex(ListListKeyIndex table, long key) ;

#define newHashMap(scope, bucketSize) \
        { \
                newIndexTable(scope, bucketSize), \
                newList(scope), \
                newList(scope), \
                scope \
        }

#define hashMapSize(m) (m.keys != NULL ? listSize(m.keys) : 0)

#define hashMapContainsKey(m, key) (indexTableGetIndex(m._table, key) >= 0)

#define hashMapGet(m, key) ({int index = indexTableGetIndex(m._table, key); m.values->items[index]; })

#define hashMapGetOrDefault(m, key, defaultValue) \
        ({int index = indexTableGetIndex(m._table, key); (index >= 0)? m.values->items[index] : defaultValue;})

#define hashMapPut(m, key, value)  \
{ \
    int index = indexTableGetIndex(m._table, key); \
        if(index >= 0)  \
        { \
                m.values->items[index] = value; \
        } \
    KeyIndex ki = {key, m.keys->_size}; \
    int bucketSize = listSize(m._table);\
    listPush(m._table->items[(unsigned long) key % bucketSize], ki);\
        listPush(m.keys, key); \
        listPush(m.values, value); \
}

#endif
