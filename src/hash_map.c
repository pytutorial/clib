#include "hash_map.h"

ListListKeyIndex newIndexTable(Scope scope, bucketSize) 
{
	ListListKeyIndex table = newList(scope);
	for(int i = 0; i < bucketSize; i++)
    {
        listPush(table, newList(scope));
    }
	return table;
}

int indexTableGetIndex(ListListKeyIndex table, int key) 
{
	if(table == NULL) return -1;
	
	int bucketSize = listSize(table);
    int index = (unsigned long) key % bucketSize;
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

#define newHashMap(scope, bucketSize) \
	{ \
		newIndexTable(scope, bucketSize), \
		newList(scope), \
		newList(scope), \
		scope \
	}

#define hashMapSize(HashMap m) (m.keys != NULL ? listSize(m.keys) : 0)

#define hashMapContainsKey(m, key) (indexTableGetIndex(m._table, key) >= 0)

#define hashMapGet(m, key) ({index = indexTableGetIndex(m._table, key); m.values.items[index]; })

#define hashMapGetOrDefault(m, key, defaultValue) \
	({index = indexTableGetIndex(m._table, key); (index >= 0)? m.values.items[index] : defaultValue;})

#define hashMapPut(m, key, value)  \
{ \
    int index = indexTableGetIndex(m._table, key); \
	if(index >= 0)  \
	{ \
		m.values.items[index] = value; \
	} \
    KeyIndex ki = {key, m.keys->_size}; \
    listPush(m._table->items[(unsigned long) key % bucketSize], ki);\
	listPush(m.keys, key); \
	listPush(m.values, value); \
}