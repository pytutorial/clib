#include <stdio.h>
#include <stdlib.h>
#include "object.h"
#include "list.h"
#include "string_util.h"

DECLARE_OBJECT(Integer, int);

void print_int(int x) { printf("%d", x); }

int main()
{
    Scope scope = newScope();

    Integer p = newInteger(scope);
    *raw_ptr(p) = 100;

    ListInt lst = newListInt(scope, 10);

    for (int i = 0; i < list_size(lst); i++)
    {
        list_at(lst, i) = i;
    }

    printf("%d\n", list_pop(lst));

    print_list(lst, print_int);

    printf("%d\n", *raw_ptr(p));

    free_scope(scope);

    return 0;
}