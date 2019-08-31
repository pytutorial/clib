#include "map.h"

int main()
{
    Scope* scope = new_scope();
    MapIntInt m1 = new_map_int_int(scope);
    MapStringInt m2 = new_map_string_int(scope);
    MapIntString m3 = new_map_int_string(scope);
    
    map_put(m1, 5, 10);
    map_put(m2, new_string(scope, "two"), 2);
    map_put(m3, 10, new_string(scope, "ten"));
    
    int value1; map_get(m1, 5, &value1, -1);
    int value2; map_get(m2, new_string(scope, "two"), &value2, -1);
    
    String value3;
    map_get(m3, 10, &value3, new_string(scope, ""));

    printf("%d %d\n", value1, value2);
    
    print_string(value3);
        
    free_scope(scope);
}