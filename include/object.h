#ifndef __OBJECT__H_
#define __OBJECT__H_
#include <string.h>
#include <stdio.h>

typedef int bool;
#define TRUE 1
#define FALSE 0

#define VALID 0x55555555
#define INVALID -1

typedef union {
    int value;
    void *ptr;
} ErrorResult;

typedef struct _ScopeData
{
    void **_items;
    int _size;
    int _capacity;
    int state;
} * Scope;

#define ERR_RAW_PTR                             "Get pointer of invalid object"
#define ERR_ATTR                                "Get attribute of invalid object"

#define _ObjectDataType(Object)                 Object##Data
#define _newObjectFunc(Object)                  new##Object
#define _makeObjectFunc(Object)                 make##Object

#define OBJ_TYPE_DEF(Object, T)                                                \
    typedef struct _ObjectDataType(Object)                                     \
    {                                                                          \
        T _data;                                                               \
        Scope scope;                                                           \
    }                                                                          \
    *Object;                                                                   \
                                                                               \
    inline static Object _newObjectFunc(Object)(Scope scope)                   \
    {                                                                          \
        Object obj = zeroAlloc(scope, sizeof(struct _ObjectDataType(Object))); \
        obj->scope = scope;                                                    \
        return obj;                                                            \
    }

#define _isValidObject(obj) ((obj) != NULL && (obj)->scope != NULL && (obj)->scope->state == VALID)

#define objectRawPtr(obj) (_isValidObject(obj) ? &((obj)->_data) : (typeof(&((obj)->_data))) QUIT(ERR_RAW_PTR, __FILE__, __LINE__).ptr)

#define attr(obj, field) (_isValidObject(obj) ? &((obj)->_data) : (typeof(&((obj)->_data))) QUIT(ERR_RAW_PTR, __FILE__, __LINE__).ptr)->field

ErrorResult QUIT(const char *message, const char *fileName, int line);

Scope newScope();

void *zeroAlloc(Scope scope, int size);

void *memRealloc(Scope scope, void *ptr, int size);

void freePtr(Scope scope, void *ptr);

void *ensureCapacity(Scope scope, void *items, int itemSize, int curSize, int curCap, int newCap, int *outNewCap);

void addPtr(Scope scope, void *item);

void freeScope(Scope scope);

#endif
