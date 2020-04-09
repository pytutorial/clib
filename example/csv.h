#ifndef __CSV_H__
#define __CSV_H__

#include "list.h"

typedef List(ListDouble) ListListDouble;

ListListDouble readCsv(Scope scope, const char* fn, const char* delimiter, int skipHeader);

#endif