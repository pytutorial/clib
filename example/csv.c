#include <stdio.h>
#include "csv.h"
#include "list.h"
#include "string_util.h"

#define MAX_LINE_LENGTH 102400

ListListDouble read_csv(Scope* scope, const char* fn, const char* delimiter, int skip_header)
{
    ListListDouble table = new_list_list_double(scope, 0);

    FILE *f = fopen(fn, "rt");
    if (!f)
    {
        printf("Cannot open file \"%s\" \n", fn);
        return table;
    }

    int i = 0;

    while (f && !feof(f))
    {
        Scope *scope2 = new_scope();

        char *line = zero_alloc(scope2, MAX_LINE_LENGTH + 1);
        fgets(line, MAX_LINE_LENGTH, f);

        if (i > 0 || !skip_header)
        {
            String st = new_string(scope2, line);
            ListString lst = split_string(st, delimiter);
            int n_items = size_of_list(lst);

            if (n_items > 0)
            {
                ListDouble row = new_list_double(scope, n_items);

                for (int i = 0; i < n_items; i++)
                {
                    at_q(row, i) = atof(str_data(at_q(lst, i)));
                }

                push(table, row);
            }
        }

        i += 1;

        free_scope(scope2);
    }

    fclose(f);
    return table;
}
