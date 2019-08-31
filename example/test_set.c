#include <stdio.h>
#include "set.h"

void print_string(String st)
{
    printf("\"%s\"", str_data(st));
}

void print_int(int i)
{
    printf("%d", i);
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
    print_set(s1, print_int);

    SetString s2 = new_set_string(scope); 
    add_to_set(s2, new_string(scope, "Hello"));
    add_to_set(s2, new_string(scope, "This is a test"));

    print_set(s2, print_string);

    free_scope(scope);
    return 0;
}