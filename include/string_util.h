#ifndef __STRING_H__
#define __STRING_H__

#include <stdlib.h>
#include "object.h"
#include "list.h"

typedef struct
{
    char *data;
    int _len;
    Scope scope;
}_StringData, *String;

typedef List(String) ListString;

int stringLen(String st);

String newNString(Scope scope, const char *src, int len);

String newString(Scope scope, const char *src);

String newStringFmt(Scope scope, const char *fmt, ...);

String concatString(String st1, String st2);

String concatStringWithPtr(String st, char *p);

String subString(String st, int start, int end);

int findChar(String st, char needle);

int findCharReverse(String st, char needle);

int findString(String st, const char *needle);

int findStringReverse(String st, const char *needle);

ListString splitString(String st, const char *delimiter);

bool stringEqual(String st1, String st2);

void printString(String st);

#endif