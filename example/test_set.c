#include <stdio.h>
#include "set.h"
#include "string_util.h"

DECLARE_SET_TYPE(SetString, String, hashString, stringEqual);


void printString(String st)
{
    printf("\"%s\"", stringDataPtr(st));
}

void printInt(int i)
{
    printf("%d", i);
}

int main()
{
    Scope scope = newScope();
    
    SetInt s1 = newSetInt(scope);
    setAdd(s1, 1);
    setAdd(s1, 2);
    setAdd(s1, 1);
    printf("%s\n", setContains(s1, 2)?"TRUE" : "FALSE");

    setRemove(s1, 2);
    printSet(s1, printInt);

    ListSetIntItem li = setItems(s1);
    printList(li, printInt);

    SetString s2 = newSetString(scope); 
    setAdd(s2, newString(scope, "Hello"));
    setAdd(s2, newString(scope, "This is a test"));
    setAdd(s2, newString(scope, "Hello"));

    printSet(s2, printString);

    freeScope(scope);
    return 0;
}