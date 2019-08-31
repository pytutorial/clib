#ifndef __CSV_H__
#define __CSV_H__

#include "list.h"

DECLARE_LIST_TYPE(ListDouble, ListListDouble, new_list_list_double);

void read_csv(const char* fn, const char* delimiter, int skip_header, ListListDouble table);

#endif