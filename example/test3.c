#include <stdio.h>

#include "list.h"

void printInt(int x) { printf("%d", x); }

int main()
{
    Scope scope = newScope();

    ListInt a = newListInt(scope, 0);

    ListInt b = a;

    listAdd(b, 1);
    listAdd(b, 2);

    printList(a, printInt);

    freeScope(scope);

    return 0;
}