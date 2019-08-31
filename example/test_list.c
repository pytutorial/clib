#include <stdio.h>

#include "list.h"

int main()
{
    Scope *scope = new_scope();

    ListDouble lst = new_list_double(scope, 5);

    at(lst, 0) = 5;
    at(lst, 1) = 4;
    at(lst, 2) = 2;
    at(lst, 3) = 0;
    at(lst, 4) = 6;
    push(lst, 1);
    push(lst, 6);

    remove_list_at(lst, 6);
    remove_list_at(lst, 0);

    print_list(lst, "%0.2f");

    ListDouble lst2 = new_list_double(scope, 2);
    list_copy(lst2, 0, lst, 1, 3);

    ListDouble view = list_view(lst, 1, 3);

    push(view, 1);

    at(lst, 1) = 10;

    print_list(lst2, "%0.2f");
    print_list(view, "%0.2f");

    double sum_lst = 0, sum_view = 0, sum_sub_lst = 0;
    get_list_sum(lst, &sum_lst);
    get_list_sum(view, &sum_view);
    get_list_sum(lst2, &sum_sub_lst);

    printf("%0.2f %0.2f %0.2f", sum_lst, sum_view, sum_sub_lst);

    free_scope(scope);
    return 0;
}