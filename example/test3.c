#include <stdio.h>

#include "list.h"

void print_int(int x) { printf("%d", x); }

int main()
{
    Scope scope = newScope();

    ListInt a = newListInt(scope, 0);

    ListInt b = a;

    list_add(b, 1);
    list_add(b, 2);

    print_list(a, print_int);

    free_scope(scope);

    return 0;
}