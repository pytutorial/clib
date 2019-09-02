#ifndef __OBJECT__H_
#define __OBJECT__H_
#include <string.h>
#include <stdio.h>

typedef int bool;
#define TRUE                        1
#define FALSE                       0

#define VALID                       0x55555555
#define INVALID                     -1

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
} *Scope;

#define ERR_RAW_PTR                 "Get pointer of invalid object"
#define ERR_ATTR                    "Get attribute of invalid object"

#define _ObjectDataType(Object)     Object ## Data
#define _newObjectFunc(Object)      new ## Object
#define _makeObjectFunc(Object)     make ## Object

#define DECLARE_OBJECT(Object, T) \
    typedef struct _ObjectDataType(Object)\
    {                 \
        T *_ptr;   \
        Scope scope; \
    }*Object;\
    \
    inline static Object _newObjectFunc(Object)(Scope scope) \
    { \
        Object obj = zero_alloc(scope, sizeof(struct _ObjectDataType(Object))); \
        obj->_ptr = zero_alloc(scope, sizeof(T)); \
        obj->scope = scope; \
        return obj; \
    } \
    \
    inline static Object _makeObjectFunc(Object)(Scope scope, T* ptr) \
    { \
        Object obj = zero_alloc(scope, sizeof(struct _ObjectDataType(Object))); \
        obj->_ptr = ptr; \
        obj->scope = scope; \
        return obj; \
    } \

#define _is_valid_object(obj)    ((obj) != NULL && (obj)->scope != NULL && (obj)->scope->state == VALID)

#define raw_ptr(obj) (_is_valid_object(obj) ? \
                        (obj)->_ptr : (typeof((obj)->_ptr)) QUIT(ERR_RAW_PTR, __FILE__, __LINE__).ptr)

#define attr(obj, field) (_is_valid_object(obj) ? \
                        (obj)->_ptr : (typeof((obj)->_ptr))QUIT(ERR_RAW_PTR, __FILE__, __LINE__).ptr)->field

ErrorResult QUIT(const char *message, const char *file_name, int line);

Scope newScope();

void *zero_alloc(Scope scope, int size);

void *mem_realloc(Scope scope, void *ptr, int size);

void free_ptr(Scope scope, void *ptr);

void *ensure_capacity(Scope scope, void *items, int item_sz, int cur_size, int cur_cap, int new_cap, int *out_new_cap);

void add_ptr(Scope scope, void *item);

void free_scope(Scope scope);

#endif