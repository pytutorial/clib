#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "object.h"

#ifdef __WIN32
#include <debugapi.h>
#define DEBUG_BREAK() DebugBreak()
#else
#define DEBUG_BREAK() *((int *)0) = 0;
#endif

#ifdef __unix__
#include <execinfo.h>
#endif

ErrorResult QUIT(const char *message, const char *fileName, int line)
{
    fprintf(stderr, "Error in file %s line %d : %s\n", fileName, line, message);

#ifdef __unix__
    void *array[100];
    int size = backtrace(array, 100);
    backtrace_symbols_fd(array, size, STDOUT_FILENO);
#endif

    DEBUG_BREAK();
    exit(-1);
    ErrorResult res;
    res.value = 0;
    return res;
}

inline static void _checkValidScope(Scope scope, const char *fileName, int line)
{
    if (scope != NULL && scope->state != VALID)
    {
        QUIT("Access invalid scope", fileName, line);
    }
}

Scope newScope()
{
    Scope scope = malloc(sizeof(struct _ScopeData));
    scope->_items = malloc(0);
    scope->_capacity = scope->_size = 0;
    scope->state = VALID;
    return scope;
}

void *zeroAlloc(Scope scope, int size)
{
    _checkValidScope(scope, __FILE__, __LINE__);

    void *ptr = malloc(size);
    memset(ptr, 0, size);

    if (scope)
        addPtr(scope, ptr);

    return ptr;
}

void *memRealloc(Scope scope, void *ptr, int size)
{
    _checkValidScope(scope, __FILE__, __LINE__);

    void *new_ptr = realloc(ptr, size);

    if (scope)
    {
        for (int i = 0; i < scope->_size; i++)
        {
            if (ptr == (scope->_items)[i])
            {
                (scope->_items)[i] = new_ptr;
                break;
            }
        }
    }

    return new_ptr;
}

void freePtr(Scope scope, void *ptr)
{
    _checkValidScope(scope, __FILE__, __LINE__);

    for (int i = 0; i < scope->_size; i++)
    {
        if (ptr == (scope->_items)[i])
        {

            free(ptr);

            for (int j = i; j < scope->_size - 1; j++)
            {
                (scope->_items)[j] = (scope->_items)[j + 1];
            }

            scope->_size -= 1;

            return;
        }
    }
}

void *ensureCapacity(Scope scope, void *items, int itemSize, int curSize, int curCap, int newCap, int *outNewCap)
{
    if (curCap >= newCap)
    {
        *outNewCap = curCap;
        return items;
    }

    _checkValidScope(scope, __FILE__, __LINE__);

    int cap = curCap;

    while (cap < newCap)
    {
        cap += cap / 2 + 8;
    }

    items = memRealloc(scope, items, cap * itemSize);
    memset((char *)items + curSize * itemSize, 0, itemSize * (cap - curSize));

    *outNewCap = cap;
    return items;
}

void _ensureScopeCap(Scope scope, int cap)
{
    _checkValidScope(scope, __FILE__, __LINE__);

    if (scope->_capacity < cap)
    {
        int newCap = 0;
        scope->_items = ensureCapacity(NULL, scope->_items, sizeof(void *), scope->_size, scope->_capacity, cap, &newCap);
        scope->_capacity = newCap;
    }
}

void addPtr(Scope scope, void *item)
{
    _checkValidScope(scope, __FILE__, __LINE__);

    _ensureScopeCap(scope, 1 + scope->_size);
    (scope->_items)[scope->_size] = item;
    scope->_size += 1;
}

void freeScope(Scope scope)
{
    _checkValidScope(scope, __FILE__, __LINE__);

    for (int i = 0; i < scope->_size; i++)
    {
        void *ptr = (scope->_items)[i];
        free(ptr);
    }

    scope->state = INVALID;
    free(scope->_items);
    free(scope);
}