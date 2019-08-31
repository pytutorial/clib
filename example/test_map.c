#include "map.h"

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

    ListInt keys = new_list_int(scope, 0);
    ListInt values = new_list_int(scope, 0);
    get_map_items(m1, keys, values);

    for(int i = 0; i < size_of_list(keys); i++)
    {
        printf("%d --> %d\n", at_q(keys, i), at_q(values, i));
    }
    
    put_map(m2, new_string(scope, "two"), 2);
    put_map(m3, 10, new_string(scope, "ten"));
    
    int value1; get_map_value(m1, 5, &value1, -1);
    int value2; get_map_value(m2, new_string(scope, "two"), &value2, -1);
    
    String value3;
    get_map_value(m3, 10, &value3, new_string(scope, ""));

    printf("%d %d\n", value1, value2);
    
    print_string(value3);
        
    free_scope(scope);
}