#include <stdio.h>
#include "set.h"

void print_string(String st)
{
    printf("%s", str_data(st));
}

int main()
{
    Scope* scope = new_scope();
    
    SetInt s1 = new_set_int(scope);
    add_to_set(s1, 1);
    add_to_set(s1, 2);
    add_to_set(s1, 1);
    bool b; check_set_contain(s1, 2, &b);
    printf("%s\n", b?"TRUE" : "FALSE");

    remove_set_item(s1, 2);
    ListInt l1 = new_list_int(scope, 0);
    get_set_items(s1, l1);
    print_list(l1, "%d");

    SetString s2 = new_set_string(scope); 
    add_to_set(s2, new_string(scope, "Hello"));
    add_to_set(s2, new_string(scope, "This is a test"));

    ListString lst = new_list_string(scope, 0);
    get_set_items(s2, lst);

    print_list_obj(lst, print_string);

    free_scope(scope);
    return 0;
}