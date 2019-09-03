#include "map.h"
#include "string_util.h"

MAP_TYPE_DEF(Int_String, int, String, hashInt, intEqual);
MAP_TYPE_DEF(String_Int, String, int, hashString, stringEqual);

void printInt(int i)
{
    printf("%d", i);
}

void printString(String st)
{
    printf("\"%s\"", stringDataPtr(st));
}

void testMap(MapInt_Int m)
{
    printf("%d %d\n", mapGet(m, 6, -1), mapGet(m, 8, -1));
}

void printItemInt_String(ItemInt_String item)
{
    printf("(%d, \"%s\")", item.key, stringDataPtr(item.value));
}

int main()
{    
    Scope scope = newScope();
    MapInt_Int m1 = newMapInt_Int(scope);
    MapString_Int m2 = newMapString_Int(scope);
    MapInt_String m3 = newMapInt_String(scope);
        
    mapPut(m1, 2, 1);
    mapPut(m1, 4, 2);
    mapPut(m1, 4, 3);
    mapPut(m1, 6, 3);
    mapPut(m1, 8, 4);
    mapPut(m1, 10, 5);

    mapRemove(m1, 8);

    printf("%d %d\n", mapContainsKey(m1, 2), mapContainsKey(m1, 3));

    testMap(m1);    

    printMap(m1, printInt, printInt);

    mapPut(m2, newString(scope, "one"), 1);
    mapPut(m2, newString(scope, "two"), 2);
    mapPut(m2, newString(scope, "three"), 3);
    printMap(m2, printString, printInt);

    mapPut(m3, 8, newString(scope, "eight"));
    mapPut(m3, 9, newString(scope, "night"));
    mapPut(m3, 10, newString(scope, "ten"));
    printMap(m3, printInt, printString);
    
    printf("%d %d\n", mapGet(m1, 5, -1), mapGet(m2, newString(scope, "two"), -1));
    
    printString(mapGet(m3, 10, newString(scope, "")));
    printf("\n");

    ListItemInt_String items3 = mapItems(m3);
    printList(items3, printItemInt_String);

    freeScope(scope);
}