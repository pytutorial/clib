#include "list.h"
#include <stdio.h>

int main()
{
    Scope scope = newScope();
    ListInt lst = newList(scope);
    listPush(lst, 1);
    listPush(lst, 3);
    listPush(lst, 8);
    listPush(lst, 6);
    printList(lst, "%d");
    printf("%d\n", listMax(lst));
    freeScope(scope);
    return 0;
}