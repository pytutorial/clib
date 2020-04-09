#include <stdio.h>
#include <string.h>

#include "list.h"
#include "string_util.h"

int main()
{
    Scope scope = newScope();

    String st = newString(scope, "Hello ");
    String st2 = newStringFmt(scope, "%s %d", "world", 1);
    String st3 = concatString(st, st2);

    printString(st);
    printString(st2);
    printString(st3);

    char *token = "Hello";
    int pos = findString(st3, token);
    if (pos >= 0)
    {
        String st4 = subString(st3, pos + strlen(token), stringLen(st3));
        printString(st4);
    }
    else
    {
        printf("Sub string not found\n");
    }

    String st5 = newString(scope, "a1,b2,c3,d4,,,,");
    ListString lst = splitString(st5, ",");

    for (int i = 0; i < listSize(lst); i++)
    {
        printString(lst->items[i]);
    }

    freeScope(scope);
}