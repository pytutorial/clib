#ifndef __HASH_MAP_H__
#define __HASH_MAP_H__

#include "list.h"
#include "scope.h"

typedef struct 
{
    unsigned long key;
    int index;
} KeyIndex;

typedef List(KeyIndex) ListKeyIndex;
typedef List(ListKeyIndex) ListListKeyIndex;

#define HashMap(K, V) struct { ListListKeyIndex _table; List(K) keys; List(V) values; Scope scope; }

inline static ListListKeyIndex newIndexTable(Scope scope, int bucketSize)  
{
    ListListKeyIndex table = newList(scope);
    for(int i = 0; i < bucketSize; i++)
    {
        listPush(table, newList(scope));
    }
    return table;
}

inline static int indexTableGetIndex(ListListKeyIndex table, unsigned long key)  
{
    if(table == NULL) return -1;
    
    int bucketSize = listSize(table);
    int index = key % bucketSize;
    ListKeyIndex lst = table->items[index];
   
    for(int i = 0; i < lst->_size; i++) 
    {
        if(lst->items[i].key == key)
        {
            return lst->items[i].index;
        }
    }
    return -1;
}

#define newHashMap(scope, bucketSize)                                           \
        {                                                                       \
                newIndexTable(scope, bucketSize),                               \
                newList(scope),                                                 \
                newList(scope),                                                 \
                scope                                                           \
        }

#define hashMapSize(m) ((m).keys != NULL ? listSize((m).keys) : 0)

#define hashMapContainsKey(m, key) (indexTableGetIndex((m)._table, (unsigned long) (key)) >= 0)

#define hashMapGet(m, key) ({int index = indexTableGetIndex((m)._table, (unsigned long) (key)); (m).values->items[(unsigned) index]; })

#define hashMapGetOrDefault(m, key, defaultValue)                               \
    ({                                                                          \
        int index = indexTableGetIndex(m._table, key);                          \
        (index >= 0)? (m).values->items[index] : defaultValue;                  \
    })

#define hashMapPut(m, key, value)                                               \
{                                                                               \
    int index = indexTableGetIndex((m)._table, key);                            \
    if(index >= 0)                                                              \
    {                                                                           \
        (m).values->items[index] = value;                                       \
    }                                                                           \
    else                                                                        \
    {                                                                           \
        KeyIndex ki = {key, (m).keys->_size};                                   \
        int bucketSize = listSize((m)._table);                                  \
        listPush((m)._table->items[(unsigned long) key % bucketSize], ki);      \
        listPush((m).keys, key);                                                \
        listPush((m).values, value);                                            \
    }                                                                           \
}

#define printHashMap(m, printItemfunc)                                          \
    {                                                                           \
        printf("{");                                                            \
                                                                                \
        for (int i = 0; i < hashMapSize(m); i++)                                \
        {                                                                       \
            printItemfunc((m).keys->items[i], (m).values->items[i]);            \
            if (i < hashMapSize(m) - 1)                                         \
                printf(", ");                                                   \
        }                                                                       \
                                                                                \
        printf("}");                                                            \
    }

#define printHashMapFmt(m, fmt)                                                 \
    {                                                                           \
        printf("{");                                                            \
                                                                                \
        for (int i = 0; i < hashMapSize(m); i++)                                \
        {                                                                       \
            printf(fmt, (m).keys->items[i], (m).values->items[i]);              \
            if (i < hashMapSize(m) - 1)                                         \
                printf(", ");                                                   \
        }                                                                       \
                                                                                \
        printf("}");                                                            \
    }

typedef HashMap(long, ListInt) HashMapLong_ListInt;
#endif
