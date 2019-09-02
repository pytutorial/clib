#include <stdio.h>

#include "list.h"

void printDouble(double x) { printf("%0.2f", x); }

int main()
{
    Scope scope = newScope();

    ListDouble lst = newListDouble(scope, 5);

    listAt(lst, 0) = 5;
    listAt(lst, 1) = 4;
    listAt(lst, 2) = 2;
    listAt(lst, 3) = 0;
    listAt(lst, 4) = 6;

    
    listAdd(lst, 1);
    listAdd(lst, 6);

    listRemoveAt(lst, 6);
    listRemoveAt(lst, 0);

    printList(lst, printDouble);

    ListDouble lst2 = newListDouble(scope, 2);
    listCopy(lst2, 0, lst, 1, 3);

    ListDouble slice = listSlice(lst, 1, 3);

    //listAdd(slice, 1);

    listAt(lst, 1) = 10;

    printList(lst2, printDouble);
    printList(slice, printDouble);
   
    freeScope(scope);
    return 0;
}