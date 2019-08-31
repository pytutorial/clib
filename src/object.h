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

typedef struct
{
    void **_items;
    int _size;
    int _capacity;
    int state;
} Scope;

#define ERR_RAW_PTR "Get pointer of invalid object"
#define ERR_ATTR "Get attribute of invalid object"

#define Object(type)  \
    struct            \
    {                 \
        type *_ptr;   \
        int _state;   \
        Scope *scope; \
    }

#define raw_ptr(obj) (((obj)._state == VALID && (obj).scope->state == VALID) ? (obj)._ptr : (typeof(obj._ptr))QUIT(ERR_RAW_PTR, __FILE__, __LINE__).ptr)

#define attr(obj, field) (((obj)._state == VALID && (obj).scope->state == VALID) ? (obj)._ptr : (typeof(obj._ptr))QUIT(ERR_RAW_PTR, __FILE__, __LINE__).ptr)->field

#define new_object(type, scope)                       \
    {                                                 \
        zero_alloc(scope, sizeof(type)), VALID, scope \
    }

#define null_object(scope)   \
    {                        \
        null, INVALID, scope \
    }
    
#define make_object(scope, ptr) \
    {                           \
        ptr, VALID, scope       \
    }

ErrorResult QUIT(const char *message, const char *file_name, int line);

Scope *new_scope();

void *zero_alloc(Scope *scope, int size);

void *mem_realloc(Scope *scope, void *ptr, int size);

int *p_int_alloc(Scope *scope, int value);

void *p_ref_alloc(Scope *scope, void *ptr);

void *p_data_alloc(Scope *scope, int len);

void free_ptr(Scope *scope, void *ptr);

void *ensure_capacity(Scope *scope, void *items, int item_sz, int cur_size, int cur_cap, int new_cap, int *out_new_cap);

void add_ptr(Scope *scope, void *item);

void free_scope(Scope *scope);

#endif