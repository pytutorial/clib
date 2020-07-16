#include "hash_map.h"

int main()
{
    Scope scope = newScope();
    HashMap m = newHashMap(scope, 1024);
    hashMapPut(m, 100, 10);
    printf("%ld\n", hashMapGet(m, 100));
    freeScope(scope);
}
