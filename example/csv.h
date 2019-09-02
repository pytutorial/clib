#ifndef __CSV_H__
#define __CSV_H__

#include "list.h"

DECLARE_LIST(ListListDouble, ListDouble);

ListListDouble read_csv(Scope scope, const char* fn, const char* delimiter, int skip_header);

#endif