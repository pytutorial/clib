#include <stdio.h>
#include <stdlib.h>
#include "object.h"
#include "list.h"
#include "string_util.h"

DECLARE_OBJECT(Integer, int);

void printInt(int x) { printf("%d", x); }

int main()
{
    Scope scope = newScope();

    Integer p = newInteger(scope);
    *objectRawPtr(p) = 100;

    ListInt lst = newListInt(scope, 10);

    for (int i = 0; i < listSize(lst); i++)
    {
        listAt(lst, i) = i;
    }

    printf("%d\n", listPop(lst));

    printList(lst, printInt);

    printf("%d\n", *objectRawPtr(p));

    freeScope(scope);

    return 0;
}