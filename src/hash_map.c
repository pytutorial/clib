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