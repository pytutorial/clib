#include <stdio.h>

#include "list.h"

void print_double(double x) { printf("%0.2f", x); }

int main()
{
    Scope scope = newScope();

    ListDouble lst = newListDouble(scope, 5);

    list_at(lst, 0) = 5;
    list_at(lst, 1) = 4;
    list_at(lst, 2) = 2;
    list_at(lst, 3) = 0;
    list_at(lst, 4) = 6;

    
    list_add(lst, 1);
    list_add(lst, 6);

    list_remove_at(lst, 6);
    list_remove_at(lst, 0);

    print_list(lst, print_double);

    ListDouble lst2 = newListDouble(scope, 2);
    list_copy(lst2, 0, lst, 1, 3);

    ListDouble slice = list_slice(lst, 1, 3);

    //list_add(slice, 1);

    list_at(lst, 1) = 10;

    print_list(lst2, print_double);
    print_list(slice, print_double);
   
    free_scope(scope);
    return 0;
}