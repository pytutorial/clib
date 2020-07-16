#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "scope.h"

Scope newScope()
{
    Scope scope = malloc(sizeof(struct _ScopeData));
    scope->_items = malloc(0);
    scope->_capacity = scope->_size = 0;
    return scope;
}

void *zeroAlloc(Scope scope, int size)
{
    void *ptr = malloc(size);
    memset(ptr, 0, size);

    if (scope)
        addPtr(scope, ptr);

    return ptr;
}

void *memRealloc(Scope scope, void *ptr, int size)
{
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

    int cap = curCap;

    while (cap < newCap)
    {
        cap += cap / 2 + 8;
    }

    if(items != NULL) 
    {
        items = memRealloc(scope, items, cap * itemSize);
        memset((char *)items + curSize * itemSize, 0, itemSize * (cap - curSize));
    }
    else
    {
        items = zeroAlloc(scope, cap * itemSize);
    }

    *outNewCap = cap;
    return items;
}

void _ensureScopeCap(Scope scope, int cap)
{
    if (scope->_capacity < cap)
    {
        int newCap = 0;
        scope->_items = ensureCapacity(NULL, scope->_items, sizeof(void *), scope->_size, scope->_capacity, cap, &newCap);
        scope->_capacity = newCap;
    }
}

void addPtr(Scope scope, void *item)
{
    _ensureScopeCap(scope, 1 + scope->_size);
    (scope->_items)[scope->_size] = item;
    scope->_size += 1;
}

void freeScope(Scope scope)
{
    for (int i = 0; i < scope->_size; i++)
    {
        void *ptr = (scope->_items)[i];
        free(ptr);
    }

    free(scope->_items);
    free(scope);
}