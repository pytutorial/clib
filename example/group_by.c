#include "map.h"

DECLARE_LIST(ListListInt, ListInt);
DECLARE_MAP(Int2ListListInt, int, ListListInt, hash_int, int_equal);

MapInt2ListListInt group_by(ListListInt table, int feature_id)
{
    MapInt2ListListInt m = newMapInt2ListListInt(table->scope);
    for(int i = 0; i < list_size(table); i++)
    {
        ListInt row = list_at_q(table, i);
        int key = list_at(row, feature_id);

        ListListInt group_items = map_get(m, key, NULL);
        
        if(group_items == NULL)
        {
            group_items = newListListInt(table->scope, 1);
            list_at_q(group_items, 0) = row;
            map_put(m, key, group_items);
        }
        else
        {
            list_add(group_items, row);            
        }
    }

    return m;
}

ListInt new_row(Scope scope, int* _row, int M)
{
    ListInt row = newListInt(scope, M);
    memcpy(list_data_ptr(row), _row, M * sizeof(int));
    return row;
}

ListListInt new_table(Scope scope, int* _table, int N, int M)
{
    ListListInt table = newListListInt(scope, N);
    for(int i = 0; i < N; i++)
    {
        list_at_q(table, i) = new_row(scope, _table, M);
        _table += M;
    }
    return table;
}

void print_int(int i)
{
    printf("%d", i);
}

void print_row(ListInt row)
{
    print_list(row, print_int);
}

void print_table(ListListInt table)
{
    print_list(table, print_row);
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

    ListListInt table = new_table(scope, data, 8, 4);

    print_table(table);

    MapInt2ListListInt groups = group_by(table, 0);
    print_map(groups, print_int, print_table);

    free_scope(scope);
    return 0;
}