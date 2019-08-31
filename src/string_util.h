#ifndef __STRING_H__
#define __STRING_H__

#include <stdlib.h>
#include "object.h"
#include "list.h"

typedef struct
{
    char *_data;
    int _len;
    int _state;
    Scope *scope;
} String;

DECLARE_LIST_TYPE(String, ListString, new_list_string);

int str_len(String st);

char *str_data(String st);

String new_nstring(Scope *scope, const char *src, int len);

String new_string(Scope *scope, const char *src);

String new_string_fmt(Scope *scope, const char *fmt, ...);

String concat_string(String st1, String st2);

String concat_string_with_ptr(String st, char *p);

String sub_string(String st, int start, int end);

String right_string(String st, int start);

String left_string(String st, int end);

int find_char(String st, char needle);

int find_string(String st, const char *needle);

ListString split_string(String st, const char *delimiter);

bool string_equal(String st1, String st2);

void print_string(String st);

#endif