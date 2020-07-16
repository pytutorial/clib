#include "hash_map.h"

typedef HashMap(long, long) HashMapLong_Long;

int main()
{
    Scope scope = newScope();
    HashMapLong_Long m = newHashMap(scope, 1024);
    hashMapPut(m, 100, 10);
    hashMapPut(m, 200, 20);
    hashMapPut(m, 300, 30);
    printHashMap(m, "%ld=%ld");
    freeScope(scope);
}
