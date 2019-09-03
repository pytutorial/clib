#include "map.h"

LIST_TYPE_DEF(ListListInt, ListInt);
MAP_TYPE_DEF(Int_ListListInt, int, ListListInt, hashInt, intEqual);

MapInt_ListListInt groupBy(ListListInt table, int featureId)
{
    MapInt_ListListInt m = newMapInt_ListListInt(table->scope);
    for(int i = 0; i < listSize(table); i++)
    {
        ListInt row = listAtQ(table, i);
        int key = listAt(row, featureId);

        ListListInt groupItems = mapGet(m, key, NULL);
        
        if(groupItems == NULL)
        {
            groupItems = newListListInt(table->scope, 1);
            listAtQ(groupItems, 0) = row;
            mapPut(m, key, groupItems);
        }
        else
        {
            listAdd(groupItems, row);            
        }
    }

    return m;
}

ListInt newRow(Scope scope, int* _row, int M)
{
    ListInt row = newListInt(scope, M);
    memcpy(listDataPtr(row), _row, M * sizeof(int));
    return row;
}

ListListInt newTable(Scope scope, int* _table, int N, int M)
{
    ListListInt table = newListListInt(scope, N);
    for(int i = 0; i < N; i++)
    {
        listAtQ(table, i) = newRow(scope, _table, M);
        _table += M;
    }
    return table;
}

void printInt(int i)
{
    printf("%d", i);
}

void printRow(ListInt row)
{
    printList(row, printInt);
}

void printTable(ListListInt table)
{
    printList(table, printRow);
}

int main()
{
    Scope scope = newScope();
    int data[] = {
                        1, 1, 3, 4,
                        2, 2, 3, 4,
                        3, 1, 5, 6,
                        4, 2, 5, 6,
                        1, 4, 3, 4,
                        2, 3, 3, 4,
                        3, 4, 5, 6,
                        4, 3, 5, 6,
                    };

    ListListInt table = newTable(scope, data, 8, 4);

    printTable(table);

    MapInt_ListListInt groups = groupBy(table, 0);
    printMap(groups, printInt, printTable);

    freeScope(scope);
    return 0;
}