#include "map.h"

DECLARE_MAP_TYPE(MapIntString, int, String, hash_int, cmp_int, new_map_int_string);
DECLARE_MAP_TYPE(MapStringInt, String, int, hash_string, string_equal, new_map_string_int);


void print_int(int i)
{
    printf("%d", i);
}

void print_string(String st)
{
    printf("\"%s\"", str_data(st));
}

int main()
{
    Scope* scope = new_scope();
    MapIntInt m1 = new_map_int_int(scope);
    MapStringInt m2 = new_map_string_int(scope);
    MapIntString m3 = new_map_int_string(scope);
    
    
    put_map(m1, 2, 1);
    put_map(m1, 4, 2);
    put_map(m1, 4, 3);
    put_map(m1, 6, 3);
    put_map(m1, 8, 4);
    put_map(m1, 10, 5);

    remove_map_key(m1, 8);

    print_map(m1, print_int, print_int);
    
    put_map(m2, new_string(scope, "one"), 1);
    put_map(m2, new_string(scope, "two"), 2);
    put_map(m2, new_string(scope, "three"), 3);
    print_map(m2, print_string, print_int);

    put_map(m3, 8, new_string(scope, "eight"));
    put_map(m3, 9, new_string(scope, "night"));
    put_map(m3, 10, new_string(scope, "ten"));
    print_map(m3, print_int, print_string);
    
    int value1; get_map_value(m1, 5, &value1, -1);
    int value2; get_map_value(m2, new_string(scope, "two"), &value2, -1);
    
    String value3;
    get_map_value(m3, 10, &value3, new_string(scope, ""));

    printf("%d %d\n", value1, value2);
    
    print_string(value3);
        
    free_scope(scope);
}