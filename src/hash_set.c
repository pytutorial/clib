#include "hash_set.h"

HashSetLong newHashSetLong(Scope scope, int bucketSize) 
{
    HashSetLong s = zeroAlloc(scope, sizeof(_HashSetLongData));
    s->_table = newList(scope);
    s->_size = 0;    
    s->scope = scope;

    for(int i = 0; i < bucketSize; i++)
    {
        ListLong lst = newList(scope);
        listPush(s->_table, lst);
    }
    return s;
}

int hashSetLongSize(HashSetLong s) 
{
    return s->_size;
}

bool hashSetLongContains(HashSetLong s, long value)
{
    int bucketSize = listSize(s->_table);
    int index = (unsigned long) value % bucketSize;
    ListLong lstItem = s->_table->items[index];
    
    for(int i = 0; i < lstItem->_size; i++) 
    {
        if(value == lstItem->items[i]) return TRUE;
    }

    return FALSE;
}

void hashSetLongAdd(HashSetLong s, long value) 
{
    int bucketSize = listSize(s->_table);
    int index = (unsigned long) value % bucketSize;
    ListLong lstItem = s->_table->items[index];

    for (int i = 0; i < lstItem->_size; i++)   
    {
        if(value == lstItem->items[i]) return;
    }

    listPush(lstItem, value);
    s->_size += 1;
}