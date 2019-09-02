#include "map.h"
#include "string_util.h"

DECLARE_MAP(Int2String, int, String, hash_int, int_equal);
DECLARE_MAP(String2Int, String, int, hash_string, string_equal);

void print_int(int i)
{
    printf("%d", i);
}

void print_string(String st)
{
    printf("\"%s\"", str_data(st));
}

void test_map(MapInt2Int m)
{
    printf("%d %d\n", map_get(m, 6, -1), map_get(m, 8, -1));
}

int main()
{    
    Scope scope = newScope();
    MapInt2Int m1 = newMapInt2Int(scope);
    MapString2Int m2 = newMapString2Int(scope);
    MapInt2String m3 = newMapInt2String(scope);
        
    map_put(m1, 2, 1);
    map_put(m1, 4, 2);
    map_put(m1, 4, 3);
    map_put(m1, 6, 3);
    map_put(m1, 8, 4);
    map_put(m1, 10, 5);

    map_remove(m1, 8);

    printf("%d %d\n", map_contains_key(m1, 2), map_contains_key(m1, 3));

    test_map(m1);    

    print_map(m1, print_int, print_int);

    map_put(m2, newString(scope, "one"), 1);
    map_put(m2, newString(scope, "two"), 2);
    map_put(m2, newString(scope, "three"), 3);
    print_map(m2, print_string, print_int);

    map_put(m3, 8, newString(scope, "eight"));
    map_put(m3, 9, newString(scope, "night"));
    map_put(m3, 10, newString(scope, "ten"));
    print_map(m3, print_int, print_string);
    
    printf("%d %d\n", map_get(m1, 5, -1), map_get(m2, newString(scope, "two"), -1));
    
    print_string(map_get(m3, 10, newString(scope, "")));
    printf("\n");

    free_scope(scope);
}