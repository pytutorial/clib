#include <stdio.h>
#include <string.h>

#include "list.h"
#include "string_util.h"

void print_string(String st)
{
    puts(str_data(st));
}

int main()
{
    Scope *scope = new_scope();

    String st = new_string(scope, "Hello ");
    String st2 = new_string_fmt(scope, "%s %d", "world", 1);
    String st3 = concat_string(st, st2);

    print_string(st);
    print_string(st2);
    print_string(st3);

    char *token = "Hello";
    int pos = find_string(st3, token);
    if (pos >= 0)
    {
        String st4 = right_string(st3, pos + strlen(token));
        print_string(st4);
    }
    else
    {
        printf("Sub string not found\n");
    }

    String st5 = new_string(scope, "a1,b2,c3,d4,,,,");
    ListString items = split_string(st5, ",");

    for (int i = 0; i < size_of_list(items); i++)
    {
        print_string(at(items, i));
    }

    free_scope(scope);
}