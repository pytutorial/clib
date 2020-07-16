#ifndef __OBJECT__H_
#define __OBJECT__H_
#include <string.h>
#include <stdio.h>

#define halt()  {*((int*)0) = 0;}

typedef int bool;
#define TRUE 1
#define FALSE 0

typedef struct _ScopeData
{
    void **_items;
    int _size;
    int _capacity;
} * Scope;

Scope newScope();

void *zeroAlloc(Scope scope, int size);

void *memRealloc(Scope scope, void *ptr, int size);

void freePtr(Scope scope, void *ptr);

void *ensureCapacity(Scope scope, void *items, int itemSize, int curSize, int curCap, int newCap, int *outNewCap);

void addPtr(Scope scope, void *item);

void freeScope(Scope scope);

#endif
