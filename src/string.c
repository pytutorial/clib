#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "string_util.h"
#include "object.h"

inline static void _checkValidString(String st, const char *fileName, int line)
{
    if (st->scope->state != VALID)
    {
        QUIT("Access invalid string", fileName, line);
    }
}

int stringLen(String st)
{
    _checkValidString(st, __FILE__, __LINE__);
    return st->_len;
}

char *stringDataPtr(String st)
{
    _checkValidString(st, __FILE__, __LINE__);

    return st->_data;
}

String newNString(Scope scope, const char *src, int len)
{
    char *data = zeroAlloc(scope, len + 1);
    strncpy(data, src, len);
    data[len] = 0;
    String st = zeroAlloc(scope, sizeof(struct _StringData));
    st->_data = data;
    st->_len = len;
    st->scope = scope;
    return st;
}

String newString(Scope scope, const char *src)
{
    return newNString(scope, src, strlen(src));
}

char *_vsfmt(Scope scope, const char *fmt, va_list arglist, int *out_length)
{
    int len = vsnprintf(NULL, 0, fmt, arglist);
    char *data = zeroAlloc(scope, len + 1);
    vsprintf(data, fmt, arglist);
    *out_length = len;
    return data;
}

String newStringFmt(Scope scope, const char *fmt, ...)
{
    va_list arglist;
    va_start(arglist, fmt);
    int len;
    char *data = _vsfmt(scope, fmt, arglist, &len);
    va_end(arglist);

    String st = zeroAlloc(scope, sizeof(struct _StringData));
    st->_data = data;
    st->_len = len;
    st->scope = scope;
    return st;
}

String _concatStringWithPtr(String st, char *p, int len)
{
    _checkValidString(st, __FILE__, __LINE__);

    int stlen = st->_len;

    char *data = zeroAlloc(st->scope, stlen + len + 1);

    strncpy(data, st->_data, stlen);
    strncpy(data + stlen, p, len);

    data[stlen + len] = 0;

    String new_st = zeroAlloc(st->scope, sizeof(struct _StringData));
    new_st->_data = data;
    new_st->_len = stlen + len;
    new_st->scope = st->scope;
    return new_st;
}

String concatStringWithPtr(String st, char *p)
{
    return _concatStringWithPtr(st, p, strlen(p));
}

String concatString(String st1, String st2)
{
    _checkValidString(st1, __FILE__, __LINE__);

    return _concatStringWithPtr(st1, st2->_data, st2->_len);
}

String subString(String st, int start, int end)
{
    _checkValidString(st, __FILE__, __LINE__);

    if (end > st->_len)
        end = st->_len;

    if (start < 0 || end <= start)
        return newString(st->scope, "");

    return newNString(st->scope, st->_data + start, end - start);
}

String rightString(String st, int start)
{
    _checkValidString(st, __FILE__, __LINE__);

    return subString(st, start, st->_len);
}

String leftString(String st, int end)
{
    _checkValidString(st, __FILE__, __LINE__);

    return subString(st, 0, end);
}

int findChar(String st, char needle)
{
    _checkValidString(st, __FILE__, __LINE__);

    char *pos = NULL;
    char *data = st->_data;

    if (data)
        pos = strchr(data, needle);

    return pos ? pos - data : -1;
}

int findString(String st, const char *needle)
{
    _checkValidString(st, __FILE__, __LINE__);

    char *pos = NULL;
    char *data = st->_data;

    if (data)
        pos = strstr(data, needle);

    return pos ? pos - data : -1;
}

ListString splitString(String st, const char *delimiter)
{
    _checkValidString(st, __FILE__, __LINE__);

    ListString v = newListString(st->scope, 0);

    int delimiter_len = strlen(delimiter);

    char *ptr = st->_data;
    char *pos;

    while ((pos = strstr(ptr, delimiter)))
    {
        int len = pos - ptr;
        if (len > 0)
            listAdd(v, newNString(st->scope, ptr, len));

        ptr = pos + delimiter_len;
    }

    if (*ptr)
        listAdd(v, newString(st->scope, ptr));

    return v;
}

bool stringEqual(String st1, String st2)
{
    return st1->_len == st2->_len && strncmp(st1->_data, st2->_data, st1->_len) == 0;
}

int hashString(String st)
{
    int *p = (int *)(stringDataPtr(st));
    int hash = 0;
    for (int i = 0; i < stringLen(st) / 4; i++)
        hash |= *p++;
    return hash;
}