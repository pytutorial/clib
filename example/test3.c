#include <stdio.h>

#include "list.h"

int main()
{
    Scope *scope = new_scope();

    ListInt a = new_list_int(scope, 0);

    ListInt b = a;

    push(b, 1);
    push(b, 2);

    print_list(a, "%d");

    free_scope(scope);

    return 0;
}