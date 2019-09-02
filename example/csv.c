#include <stdio.h>
#include "csv.h"
#include "list.h"
#include "string_util.h"

#define MAX_LINE_LENGTH 102400

ListListDouble read_csv(Scope scope, const char* fn, const char* delimiter, int skip_header)
{
    ListListDouble table = newListListDouble(scope, 0);

    FILE *f = fopen(fn, "rt");
    if (!f)
    {
        printf("Cannot open file \"%s\" \n", fn);
        return table;
    }

    int i = 0;

    while (f && !feof(f))
    {
        Scope scope2 = newScope();

        char *line = zero_alloc(scope2, MAX_LINE_LENGTH + 1);
        fgets(line, MAX_LINE_LENGTH, f);

        if (i > 0 || !skip_header)
        {
            String st = newString(scope2, line);
            ListString lst = split_string(st, delimiter);
            int n_items = list_size(lst);

            if (n_items > 0)
            {
                ListDouble row = newListDouble(scope, n_items);

                for (int i = 0; i < n_items; i++)
                {
                    list_at_q(row, i) = atof(str_data(list_at_q(lst, i)));
                }

                list_add(table, row);
            }
        }

        i += 1;

        free_scope(scope2);
    }

    fclose(f);
    return table;
}
