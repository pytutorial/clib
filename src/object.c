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

ErrorResult QUIT(const char *message, const char *file_name, int line)
{
    fprintf(stderr, "Error in file %s line %d : %s\n", file_name, line, message);

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

inline static void _check_valid_scope(Scope scope, const char *file_name, int line)
{
    if (scope != NULL && scope->state != VALID)
    {
        QUIT("Access invalid scope", file_name, line);
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

void *zero_alloc(Scope scope, int size)
{
    _check_valid_scope(scope, __FILE__, __LINE__);

    void *ptr = malloc(size);
    memset(ptr, 0, size);

    if (scope)
        add_ptr(scope, ptr);

    return ptr;
}

void *mem_realloc(Scope scope, void *ptr, int size)
{
    _check_valid_scope(scope, __FILE__, __LINE__);

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

void free_ptr(Scope scope, void *ptr)
{
    _check_valid_scope(scope, __FILE__, __LINE__);

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

void *ensure_capacity(Scope scope, void *items, int item_sz, int cur_size, int cur_cap, int new_cap, int *out_new_cap)
{
    if (cur_cap >= new_cap)
    {
        *out_new_cap = cur_cap;
        return items;
    }

    _check_valid_scope(scope, __FILE__, __LINE__);

    int cap = cur_cap;

    while (cap < new_cap)
    {
        cap += cap / 2 + 8;
    }

    items = mem_realloc(scope, items, cap * item_sz);
    memset((char *)items + cur_size * item_sz, 0, item_sz * (cap - cur_size));

    *out_new_cap = cap;
    return items;
}

void _ensure_scope_cap(Scope scope, int cap)
{
    _check_valid_scope(scope, __FILE__, __LINE__);

    if (scope->_capacity < cap)
    {
        int newCap = 0;
        scope->_items = ensure_capacity(NULL, scope->_items, sizeof(void *), scope->_size, scope->_capacity, cap, &newCap);
        scope->_capacity = newCap;
    }
}

void add_ptr(Scope scope, void *item)
{
    _check_valid_scope(scope, __FILE__, __LINE__);

    _ensure_scope_cap(scope, 1 + scope->_size);
    (scope->_items)[scope->_size] = item;
    scope->_size += 1;
}

void free_scope(Scope scope)
{
    _check_valid_scope(scope, __FILE__, __LINE__);

    for (int i = 0; i < scope->_size; i++)
    {
        void *ptr = (scope->_items)[i];
        free(ptr);
    }

    scope->state = INVALID;
    free(scope->_items);
    free(scope);
}