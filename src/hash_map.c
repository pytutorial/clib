#include "hash_map.h"

HashMapL2L newHashMapL2L(Scope scope, int bucketSize)
{
    HashMapL2L m = zeroAlloc(scope, sizeof(_HashMapL2LData));
    m->_table = newList(scope);
    m->_size = 0;
    m->scope = scope;

    for(int i = 0; i < bucketSize; i++)
    {
        ListHashMapItemL2L lst = newList(scope);
        listPush(m->_table, lst);
    }

    return m;
}

int hashMapL2LSize(HashMapL2L m)
{
    return m->_size;
}

bool hashMapL2LContainsKey(HashMapL2L m, long key)
{
    int bucketSize = listSize(m->_table);
    int index = (unsigned long) key % bucketSize;
    ListHashMapItemL2L lstItem = m->_table->items[index];
    
    for(int i = 0; i < lstItem->_size; i++) 
    {
        if(lstItem->items[i].key == key) return TRUE;
    }

    return FALSE;
}

void hashMapL2LPut(HashMapL2L m, long key, long value) 
{
    int bucketSize = listSize(m->_table);
    int index = (unsigned long) key % bucketSize;
    ListHashMapItemL2L lstItem = m->_table->items[index];
    
    for(int i = 0; i < lstItem->_size; i++) 
    {
        if(lstItem->items[i].key == key)
        {
            lstItem->items[i].value = value;
            return;
        }
    }

    HashMapItemL2L it = {key, value};
    listPush(lstItem, it);
}

long hashMapL2LGet(HashMapL2L m, long key, long defaultValue) 
{
    int bucketSize = listSize(m->_table);
    int index = (unsigned long) key % bucketSize;
    ListHashMapItemL2L lstItem = m->_table->items[index];

    for(int i = 0; i < lstItem->_size; i++) 
    {
        if(lstItem->items[i].key == key)
        {
            return lstItem->items[i].value;
        }
    }

    return defaultValue;
}

ListLong hashMapL2LKeys(HashMapL2L m) 
{
    ListLong lst = newList(m->scope);
    
    for(int i = 0; i < m->_table->_size; i++) 
    {
        ListHashMapItemL2L lstItem = m->_table->items[i];
        for(int j = 0; j < lstItem->_size; j++)
        {
            listPush(lst, lstItem->items[j].key);
        }
    }
    return lst;
}

ListLong hashMapL2LValues(HashMapL2L m) 
{
    ListLong lst = newList(m->scope);
    
    for(int i = 0; i < m->_table->_size; i++) 
    {
        ListHashMapItemL2L lstItem = m->_table->items[i];
        for(int j = 0; j < lstItem->_size; j++)
        {
            listPush(lst, lstItem->items[j].value);
        }
    }
    return lst;
}

ListHashMapItemL2L hashMapL2LItems(HashMapL2L m) 
{
    ListHashMapItemL2L lst = newList(m->scope);
    
    for(int i = 0; i < m->_table->_size; i++) 
    {
        ListHashMapItemL2L lstItem = m->_table->items[i];
        for(int j = 0; j < lstItem->_size; j++)
        {
            listPush(lst, lstItem->items[j]);
        }
    }
    return lst;
}


