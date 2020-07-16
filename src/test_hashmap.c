#include "hash_map.h"

typedef HashMap(long, ListLong) HashMapLong_ListLong;

void printItemLong_ListLong(long key, ListLong value)
{
    printf("%ld=", key);
    printList(value);
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
    printHashMap(m, printItemLong_ListLong);
    printf("\n");
    freeScope(scope);
}
