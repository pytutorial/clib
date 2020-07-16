#include "list.h"
#include <stdio.h>

int main()
{
    Scope scope = newScope();
    ListInt lst = newList(scope);
    listPush(lst, 1);
    listPush(lst, 3);
    listPush(lst, 6);
    listPush(lst, 8);
    printList(lst, "%d");
    freeScope(scope);
    return 0;
}