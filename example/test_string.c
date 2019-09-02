#include <stdio.h>
#include <string.h>

#include "list.h"
#include "string_util.h"

void printString(String st)
{
    puts(stringDataPtr(st));
}

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
        String st4 = rightString(st3, pos + strlen(token));
        printString(st4);
    }
    else
    {
        printf("Sub string not found\n");
    }

    String st5 = newString(scope, "a1,b2,c3,d4,,,,");
    ListString items = splitString(st5, ",");

    for (int i = 0; i < listSize(items); i++)
    {
        printString(listAt(items, i));
    }

    freeScope(scope);
}