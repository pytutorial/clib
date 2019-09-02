#ifndef __LIST_H__
#define __LIST_H__
#include <stdio.h>
#include <stdlib.h>
#include "object.h"

#define ALWAYS_BOUND_CHECK 0

#define ERR_INVALID_LIST "Access an invalid list"
#define ERR_LIST_AT "Get element of an invalid list or index out of range"
#define ERR_POP_LIST "Pop an empty or invalid list"
#define ERR_LIST_INDEX_OUT_OF_RANGE "List index out of range"
#define ERR_MODIFY_READONLY_LIST "Modify a readonly list"
#define ERR_INVALID_LIST_SLICE "Get slice of invalid list or index out of range"
#define ERR_SOURCE_INDEX "Source index out of range"
#define ERR_DEST_INDEX "Destination index out of range"
#define ERR_SRC_DST_TYPE "Source and destination list having different types"

#define _ListTypeData(ListType) _##ListType##Data
#define _ensureListCapFunc(ListType) _ensure##ListType##Cap
#define _listAddFunc(ListType) _##ListType##Add
#define _listResizeFunc(ListType) _##ListType##Resize
#define _listPopFunc(ListType) _##ListType##Pop
#define _listRemoveAtFunc(ListType) _##ListType##RemoveAt
#define _listClearFunc(ListType) _##ListType##Clear
#define _listSliceFunc(ListType) _##ListType##Slice
#define _printListFunc(ListType) _print##ListType
#define _newListFunc(ListType) new##ListType

#define _DECLARE_LIST_TYPE(ListType, T)                                                                       \
    typedef struct _ListTypeData(ListType)                                                                    \
    {                                                                                                         \
        T *_items;                                                                                            \
        int _size;                                                                                            \
        int _capacity;                                                                                        \
        int _readonly;                                                                                        \
        void (*_add)(struct _ListTypeData(ListType) * lst, T item);                                           \
        T (*_pop)                                                                                             \
        (struct _ListTypeData(ListType) * lst);                                                               \
        void (*_resize)(struct _ListTypeData(ListType) * lst, int size);                                      \
        void (*_remove_at)(struct _ListTypeData(ListType) * lst, int index);                                  \
        void (*_clear)(struct _ListTypeData(ListType) * lst);                                                 \
        struct _ListTypeData(ListType) * (*_slice)(struct _ListTypeData(ListType) * lst, int start, int end); \
        void (*_print)(struct _ListTypeData(ListType) * lst, void (*print_item_func)(T item));                \
        Scope scope;                                                                                          \
    }                                                                                                         \
    *ListType;

#define _DECLARE_LIST_ENSURE_CAP(ListType, T)                              \
    inline static void _ensureListCapFunc(ListType)(ListType lst, int cap) \
    {                                                                      \
        if (lst->_capacity < cap)                                          \
        {                                                                  \
            int new_cap = 0;                                               \
            lst->_items =                                                  \
                ensure_capacity(lst->scope,                                \
                                lst->_items,                               \
                                sizeof(T),                                 \
                                lst->_size,                                \
                                lst->_capacity,                            \
                                cap,                                       \
                                &new_cap);                                 \
                                                                           \
            lst->_capacity = new_cap;                                      \
        }                                                                  \
    }

#define _DECLARE_LIST_ADD(ListType, T)                              \
    inline static void _listAddFunc(ListType)(ListType lst, T item) \
    {                                                               \
        _ensureListCapFunc(ListType)(lst, 1 + lst->_size);          \
        lst->_items[lst->_size] = item;                             \
        lst->_size += 1;                                            \
    }

#define _DECLARE_LIST_RESIZE(ListType, T)                                \
    inline static void _listResizeFunc(ListType)(ListType lst, int size) \
    {                                                                    \
        _ensureListCapFunc(ListType)(lst, size);                         \
        lst->_size = size;                                               \
    }

#define _DECLARE_LIST_POP(ListType, T)                   \
    inline static T _listPopFunc(ListType)(ListType lst) \
    {                                                    \
        return lst->_items[--lst->_size];                \
    }

#define _DECLARE_LIST_REMOVE_AT(ListType, T)                                \
    inline static void _listRemoveAtFunc(ListType)(ListType lst, int index) \
    {                                                                       \
        for (int i = index; i < lst->_size - 1; i++)                        \
        {                                                                   \
            lst->_items[i] = lst->_items[i + 1];                            \
        }                                                                   \
        lst->_size -= 1;                                                    \
    }

#define _DECLARE_LIST_CLEAR(ListType, T)                      \
    inline static void _listClearFunc(ListType)(ListType lst) \
    {                                                         \
        _listResizeFunc(ListType)(lst, 0);                    \
    }

#define _DECLARE_LIST_SLICE(ListType, T)                                                 \
    inline static ListType _listSliceFunc(ListType)(ListType lst, int start, int end)    \
    {                                                                                    \
        ListType slice = zero_alloc(lst->scope, sizeof(struct _ListTypeData(ListType))); \
        slice->_items = lst->_items + start;                                             \
        slice->_size = slice->_capacity = end - start;                                   \
        slice->_readonly = TRUE;                                                         \
        slice->_add = NULL;                                                              \
        slice->_pop = NULL;                                                              \
        slice->_resize = NULL;                                                           \
        slice->_remove_at = NULL;                                                        \
        slice->_clear = NULL;                                                            \
        slice->_slice = lst->_slice;                                                     \
        slice->_print = lst->_print;                                                     \
        slice->scope = lst->scope;                                                       \
        return slice;                                                                    \
    }

#define _DECLARE_LIST_PRINT(ListType, T)                                                       \
    inline static void _printListFunc(ListType)(ListType lst, void (*print_item_func)(T item)) \
    {                                                                                          \
        printf("[");                                                                           \
                                                                                               \
        for (int i = 0; i < lst->_size; i++)                                                   \
        {                                                                                      \
            print_item_func(lst->_items[i]);                                                   \
            if (i < lst->_size - 1)                                                            \
                printf(", ");                                                                  \
        }                                                                                      \
                                                                                               \
        printf("]\n");                                                                         \
    }

#define _DECLARE_NEW_LIST(ListType, T)                                            \
    inline static ListType _newListFunc(ListType)(Scope scope, int size)          \
    {                                                                             \
        ListType lst = zero_alloc(scope, sizeof(struct _ListTypeData(ListType))); \
        lst->_items = zero_alloc(scope, size * sizeof(T));                        \
        lst->_size = lst->_capacity = size;                                       \
        lst->_readonly = FALSE;                                                   \
        lst->_add = _listAddFunc(ListType);                                       \
        lst->_pop = _listPopFunc(ListType);                                       \
        lst->_resize = _listResizeFunc(ListType);                                 \
        lst->_remove_at = _listRemoveAtFunc(ListType);                            \
        lst->_clear = _listClearFunc(ListType);                                   \
        lst->_slice = _listSliceFunc(ListType);                                   \
        lst->_print = _printListFunc(ListType);                                   \
        lst->scope = scope;                                                       \
        return lst;                                                               \
    }

#define DECLARE_LIST(ListType, T)         \
    _DECLARE_LIST_TYPE(ListType, T)       \
    _DECLARE_LIST_ENSURE_CAP(ListType, T) \
    _DECLARE_LIST_ADD(ListType, T)        \
    _DECLARE_LIST_RESIZE(ListType, T)     \
    _DECLARE_LIST_POP(ListType, T)        \
    _DECLARE_LIST_REMOVE_AT(ListType, T)  \
    _DECLARE_LIST_CLEAR(ListType, T)      \
    _DECLARE_LIST_SLICE(ListType, T)      \
    _DECLARE_LIST_PRINT(ListType, T)      \
    _DECLARE_NEW_LIST(ListType, T)

#define list_at(lst, i) ( \
    *(                    \
        ((unsigned)(i) < (unsigned)(lst)->_size && (lst)->scope->state == VALID) ? ((lst)->_items + (i)) : (typeof((lst)->_items))QUIT(ERR_LIST_AT, __FILE__, __LINE__).ptr))

#if ALWAYS_BOUND_CHECK
#define list_at_q(lst, i) list_at(lst, i)
#else
#define list_at_q(lst, i) ((lst)->_items[i])
#endif

#define _is_valid_list(lst) ((lst) != NULL && (lst)->scope != NULL && (lst)->scope->state == VALID)
#define _is_modifiable_list(lst) ((lst) != NULL && !(lst)->_readonly)

#define _check_list_valid(lst)                      \
    if (!_is_valid_list(lst))                       \
    {                                               \
        QUIT(ERR_INVALID_LIST, __FILE__, __LINE__); \
    }

#define _check_list_modifiable(lst)                         \
    if (!_is_modifiable_list(lst))                          \
    {                                                       \
        QUIT(ERR_MODIFY_READONLY_LIST, __FILE__, __LINE__); \
    }

#define list_add(lst, item)          \
    {                                \
        _check_list_valid(lst);      \
        _check_list_modifiable(lst); \
        (lst)->_add(lst, item);      \
    }

#define list_pop(lst) ( \
    (_is_valid_list(lst) && _is_modifiable_list(lst)) ? (lst)->_pop(lst) : *((typeof((lst)->_items))QUIT(ERR_POP_LIST, __FILE__, __LINE__).ptr))

#define list_remove_at(lst, index)                                 \
    {                                                              \
        _check_list_valid(lst);                                    \
        _check_list_modifiable(lst);                               \
                                                                   \
        if ((unsigned)(index) >= (lst)->_size)                     \
        {                                                          \
            QUIT(ERR_LIST_INDEX_OUT_OF_RANGE, __FILE__, __LINE__); \
        }                                                          \
                                                                   \
        (lst)->_remove_at(lst, index);                             \
    }

#define list_resize(lst, size)       \
    {                                \
        _check_list_valid(lst);      \
        _check_list_modifiable(lst); \
        (lst)->_resize(lst, size);   \
    }

#define clear_list(lst)              \
    {                                \
        _check_list_valid(lst);      \
        _check_list_modifiable(lst); \
        (lst)->_clear(lst);          \
    }

#define list_size(lst) (_is_valid_list(lst) ? (lst)->_size : QUIT(ERR_INVALID_LIST, __FILE__, __LINE__).value)

#define list_is_empty(lst) (_is_valid_list(lst) ? (lst)->_size == 0 : QUIT(ERR_INVALID_LIST, __FILE__, __LINE__).value)

#define list_data_ptr(lst) (_is_valid_list(lst) ? (lst)->_items : (typeof(lst->_items))QUIT(ERR_INVALID_LIST, __FILE__, __LINE__).ptr)

#define _check_range(lst, start, end) (start >= 0 && start <= end && end <= list_size(lst))

#define list_slice(lst, start, end) ((_is_valid_list(lst) && _check_range(lst, start, end)) ? (lst)->_slice(lst, start, end) : (typeof(lst))QUIT(ERR_INVALID_LIST_SLICE, __FILE__, __LINE__).ptr)

#define list_copy(dst, dst_start, src, src_start, src_end)               \
    {                                                                    \
        int src_size = list_size(src);                                   \
        int dst_size = list_size(dst);                                   \
        if (src_start < 0 || src_end < src_start || src_end > src_size)  \
        {                                                                \
            QUIT(ERR_SOURCE_INDEX, __FILE__, __LINE__);                  \
        }                                                                \
                                                                         \
        int len = src_end - src_start;                                   \
        if (dst_start < 0 || dst_start + len > dst_size)                 \
        {                                                                \
            QUIT(ERR_DEST_INDEX, __FILE__, __LINE__);                    \
        }                                                                \
                                                                         \
        if (sizeof(src->_items[0]) != sizeof(dst->_items[0]))            \
        {                                                                \
            QUIT(ERR_SRC_DST_TYPE, __FILE__, __LINE__);                  \
        }                                                                \
                                                                         \
        memcpy((dst)->_items + dst_start,                                \
               (src)->_items + src_start, len * sizeof(src->_items[0])); \
    }

#define print_list(lst, print_item_func)     \
    if (_is_valid_list(lst))                 \
    {                                        \
        (lst)->_print(lst, print_item_func); \
    }

DECLARE_LIST(ListInt, int);
DECLARE_LIST(ListChar, char);
DECLARE_LIST(ListLong, long);
DECLARE_LIST(ListShort, short);
DECLARE_LIST(ListDouble, double);
DECLARE_LIST(ListFloat, float);

#endif