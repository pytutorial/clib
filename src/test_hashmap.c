#include "hash_map.h"

typedef HashMap(long, ListLong) HashMapLong_ListLong;

void printItemLong_ListLong(long key, ListLong value)
{
    printf("%ld=", key);
    printListFmt(value, "%ld");
}

int main()
{
    Scope scope = newScope();
    HashMapLong_ListLong m = newHashMap(scope, 1024);
    hashMapPut(m, 10, newList(scope));
    ListLong lst = hashMapGet(m, 10);
    listPush(lst, 1);
    listPush(lst, 3);
    listPush(lst, 5);
    listPush(lst, 7);
    
    hashMapPut(m, 20, newList(scope));
    lst = hashMapGet(m, 20);
    listPush(lst, 2);
    listPush(lst, 4);
    listPush(lst, 6);
    listPush(lst, 8);
    
    printHashMap(m, printItemLong_ListLong);
    printf("\n");
    freeScope(scope);
}
