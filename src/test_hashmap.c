#include "hash_map.h"

typedef HashMap(long, ListLong) HashMapLong_ListLong;

void printItemLong_ListLong(long key, ListLong value)
{
    printf("%ld=[", key);
    for(int i = 0; i < listSize(value); i++) 
    {
        printf("%ld", value->items[i]);
        if(i+1 < listSize(value)) printf(",");
    }
    printf("]\n");    
}

int main()
{
    Scope scope = newScope();
    HashMapLong_Long m = newHashMap(scope, 1024);
    hashMapPut(m, 10, newList(scope));
    ListLong lst = hashMapGet(m, 10);
    listAdd(lst, 1);
    listAdd(lst, 3);
    listAdd(lst, 5);
    listAdd(lst, 7);
    printHashMap(m, printItemLong_ListLong);
    freeScope(scope);
}
