#include "hash_map.h"

typedef HashMap(long) HashMapLong;

int main()
{
    Scope scope = newScope();
    HashMapLong m = newHashMap(scope, 1024);
    hashMapPut(m, 100, 10);
    printf("%ld\n", hashMapGet(m, 100));
    freeScope(scope);
}
