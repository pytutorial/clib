#include <stdio.h>
#include <stdlib.h>
#include "object.h"
#include "list.h"
#include "string_util.h"

typedef Object(int) Integer;

int main()
{
    Scope *scope = new_scope();

    Integer p = new_object(int, scope);
    *raw_ptr(p) = 100;

    ListInt lst = new_list_int(scope, 10);

    for (int i = 0; i < list_size(lst); i++)
    {
        at(lst, i) = i;
    }

    int x = pop(lst);

    fprint_list(stdout, lst, "%d");

    printf("%d\n", *raw_ptr(p));

    free_scope(scope);

    return 0;
}