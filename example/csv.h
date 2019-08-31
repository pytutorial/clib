#ifndef __CSV_H__
#define __CSV_H__

#include "list.h"

DECLARE_LIST_TYPE(ListListDouble, ListDouble, new_list_list_double);

ListListDouble read_csv(Scope* scope, const char* fn, const char* delimiter, int skip_header);

#endif