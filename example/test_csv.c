#include "csv.h"

ListListDouble readCsv(Scope scope, const char* fn, const char* delimiter, bool skipHeader);

int main() 
{
    Scope scope = newScope();
    ListListDouble table = readCsv(scope, "admit.csv", ",", 1);
    
    for(int i = 0; i < listSize(table); i++) 
    {
        ListDouble row = table->items[i];
        printList(row, "%0.2f");
    }
    freeScope(scope);
}