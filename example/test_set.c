#include <stdio.h>
#include "set.h"
#include "string_util.h"

DECLARE_SET(SetString, String, hash_string, string_equal);


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
    Scope scope = newScope();
    
    SetInt s1 = newSetInt(scope);
    set_add(s1, 1);
    set_add(s1, 2);
    set_add(s1, 1);
    printf("%s\n", set_contains(s1, 2)?"TRUE" : "FALSE");

    set_remove(s1, 2);
    print_set(s1, print_int);

    ListSetIntItem li = set_items(s1);
    print_list(li, print_int);

    SetString s2 = newSetString(scope); 
    set_add(s2, newString(scope, "Hello"));
    set_add(s2, newString(scope, "This is a test"));
    set_add(s2, newString(scope, "Hello"));

    print_set(s2, print_string);

    free_scope(scope);
    return 0;
}