#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "string_util.h"
#include "object.h"

int stringLen(String st)
{
    return st->_len;
}

String newNString(Scope scope, const char *src, int len)
{
    char *data = zeroAlloc(scope, len + 1);
    strncpy(data, src, len);
    data[len] = 0;
    String st = zeroAlloc(scope, sizeof(_StringData));
    st->data = data;
    st->_len = len;
    st->scope = scope;
    return st;
}

String newString(Scope scope, const char *src)
{
    return newNString(scope, src, strlen(src));
}

String newStringFmt(Scope scope, const char *fmt, ...)
{
    va_list arglist;
    va_start(arglist, fmt);
    int len = vsnprintf(NULL, 0, fmt, arglist);
    va_end(arglist);

    char *data = zeroAlloc(scope, len + 1);

    va_start(arglist, fmt);    
    vsprintf(data, fmt, arglist);
    va_end(arglist);
    
    String st = zeroAlloc(scope, sizeof(_StringData));
    st->data = data;
    st->_len = len;
    st->scope = scope;
    return st;
}

String _concatStringWithPtr(String st, char *p, int len)
{
    int stlen = st->_len;

    char *data = zeroAlloc(st->scope, stlen + len + 1);

    strncpy(data, st->data, stlen);
    strncpy(data + stlen, p, len);

    data[stlen + len] = 0;

    String res = zeroAlloc(st->scope, sizeof(_StringData));
    res->data = data;
    res->_len = stlen + len;
    res->scope = st->scope;
    return res;
}

String concatStringWithPtr(String st, char *p)
{
    return _concatStringWithPtr(st, p, strlen(p));
}

String concatString(String st1, String st2)
{
    return _concatStringWithPtr(st1, st2->data, st2->_len);
}

String subString(String st, int start, int end)
{
    if (end > st->_len)
        end = st->_len;

    if (start < 0 || end <= start)
        return newString(st->scope, "");

    return newNString(st->scope, st->data + start, end - start);
}

String rightString(String st, int start)
{
    return subString(st, start, st->_len);
}

String leftString(String st, int end)
{
    return subString(st, 0, end);
}

int findChar(String st, char needle)
{
    char *pos = NULL;
    char *data = st->data;

    if (data)
        pos = strchr(data, needle);

    return pos ? pos - data : -1;
}

int findCharReverse(String st, char needle)
{
    char *pos = NULL;
    char *data = st->data;

    if (data)
        pos = strrchr(data, needle);

    return pos ? pos - data : -1;
}

int findString(String st, const char *needle)
{
    char *pos = NULL;
    char *data = st->data;

    if (data)
        pos = strstr(data, needle);

    return pos ? pos - data : -1;
}

char* _strReverse(const char* st) 
{
    int len = strlen(st);
    char* stRev = malloc(len+1);
    stRev[len] = 0;
    for(int i = 0; i < len; i++)
    {
        stRev[i] = st[len-1-i];
    }
    return stRev;
}

int findStringReverse(String st, const char *needle)
{
    if(st->data)
    {
        char* stRev = _strReverse(st->data);
        char* needleRev = _strReverse(needle);
        char* pos = strstr(stRev, needleRev);
        
        int ipos = -1;
        if(pos)
        {
            ipos = st->_len - strlen(needleRev) - (pos - stRev);
        }

        free(stRev);
        free(needleRev);
        return ipos;
    }
    return -1;
}

ListString splitString(String st, const char *delimiter)
{
    ListString v = (ListString) newList(st->scope);

    int delimiter_len = strlen(delimiter);

    char *ptr = st->data;
    char *pos;

    while ((pos = strstr(ptr, delimiter)))
    {
        int len = pos - ptr;
        if (len > 0)
            listPush(v, newNString(st->scope, ptr, len));

        ptr = pos + delimiter_len;
    }

    if (*ptr)
        listPush(v, newString(st->scope, ptr));

    return v;
}

bool stringEqual(String st1, String st2)
{
    return st1->_len == st2->_len && strncmp(st1->data, st2->data, st1->_len) == 0;
}

void printString(String st) 
{
    puts(st->data);
}