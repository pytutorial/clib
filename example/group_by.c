#include "map.h"

DECLARE_LIST(ListListInt, ListInt);
DECLARE_MAP_TYPE(Int2ListListInt, int, ListListInt, hashInt, intEqual);

MapInt2ListListInt groupBy(ListListInt table, int featureId)
{
    MapInt2ListListInt m = newMapInt2ListListInt(table->scope);
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

ListInt new_row(Scope scope, int* _row, int M)
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
        listAtQ(table, i) = new_row(scope, _table, M);
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

    MapInt2ListListInt groups = groupBy(table, 0);
    printMap(groups, printInt, printTable);

    freeScope(scope);
    return 0;
}