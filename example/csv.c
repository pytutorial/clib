#include <stdio.h>
#include "csv.h"
#include "list.h"
#include "string_util.h"

#define MAX_LINE_LENGTH 102400

ListListDouble readCsv(Scope scope, const char* fn, const char* delimiter, int skipHeader)
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

        char *line = zeroAlloc(scope2, MAX_LINE_LENGTH + 1);
        fgets(line, MAX_LINE_LENGTH, f);

        if (i > 0 || !skipHeader)
        {
            String st = newString(scope2, line);
            ListString lst = splitString(st, delimiter);
            int nItem = listSize(lst);

            if (nItem > 0)
            {
                ListDouble row = newListDouble(scope, nItem);

                for (int i = 0; i < nItem; i++)
                {
                    listAtQ(row, i) = atof(stringDataPtr(listAtQ(lst, i)));
                }

                listAdd(table, row);
            }
        }

        i += 1;

        freeScope(scope2);
    }

    fclose(f);
    return table;
}
