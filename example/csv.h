#ifndef __CSV_H__
#define __CSV_H__

#include "list.h"

LIST_TYPE_DEF(ListListDouble, ListDouble);

ListListDouble readCsv(Scope scope, const char* fn, const char* delimiter, int skipHeader);

#endif