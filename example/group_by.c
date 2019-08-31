#include "map.h"

DECLARE_LIST_TYPE(ListListInt, ListInt, new_list_list_int);
DECLARE_LIST_TYPE(ListListListInt, ListListInt, new_list_list_list_int);
DECLARE_MAP_TYPE(MapIntListListInt, int, ListListInt, hash_int, cmp_int, new_map_int_list_list_int);

MapIntListListInt group_by(ListListInt table, int feature_id)
{
    MapIntListListInt m = new_map_int_list_list_int(table.scope);
    for(int i = 0; i < size_of_list(table); i++)
    {
        ListInt row = at_q(table, i);
        int key = at(row, feature_id);

        ListListInt group_items;
        get_map_value(m, key, &group_items, (ListListInt) null_list);
        
        if(is_null_list(group_items))
        {
            group_items = new_list_list_int(table.scope, 1);
            at_q(group_items, 0) = row;
            put_map(m, key, group_items);
        }
        else
        {
            push(group_items, row);            
        }
    }

    return m;
}

ListInt new_row(Scope* scope, int* _row, int M)
{
    ListInt row = new_list_int(scope, M);
    memcpy(list_data_ptr(row), _row, M * sizeof(int));
    return row;
}

ListListInt new_table(Scope* scope, int* _table, int N, int M)
{
    ListListInt table = new_list_list_int(scope, N);
    for(int i = 0; i < N; i++)
    {
        at_q(table, i) = new_row(scope, _table, M);
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
    print_list(row, "%d");
}

void print_table(ListListInt table)
{
    print_list_obj(table, print_row);
}

int main()
{
    Scope* scope = new_scope();
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

    MapIntListListInt groups = group_by(table, 0);
    print_map(groups, print_int, print_table);

    free_scope(scope);
    return 0;
}