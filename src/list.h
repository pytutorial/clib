#ifndef __LIST_H__
#define __LIST_H__
#include <stdio.h>
#include <stdlib.h>
#include "object.h"

#define ALWAYS_BOUND_CHECK 0

#define ERR_AT "Invalid list or list index out of range"
#define ERR_REMOVE_AT "Remove item from list : invalid list ,or readonly list, or index out of range"
#define ERR_POP "Pop invalid or empty list"
#define ERR_LIST_SIZE "Get size of invalid list"
#define ERR_RESIZE "Resize invalid or readonly list"
#define ERR_PUSH "Push to invalid or readonly list"
#define ERR_SOURCE_INDEX "Source index out of range"
#define ERR_DEST_INDEX "Destination index out of range"
#define ERR_SRC_DST_TYPE "Source and destination list having different types"
#define ERR_LIST_VIEW "Invalid list or get view of list out of range"

#define List(type)        \
    struct                \
    {                     \
        type **_p_items;  \
        int *_p_size;     \
        int *_p_capacity; \
        int _state;       \
        int _readonly;    \
        Scope *scope;     \
    }

#define _list_size(lst) (*(lst)._p_size)

#define _list_items(lst) (*(lst)._p_items)

#define _list_cap(lst) (*(lst)._p_capacity)

#define _new_list(type, scope, size)              \
    {                                             \
        p_data_alloc(scope, size * sizeof(type)), \
            p_int_alloc(scope, size),             \
            p_int_alloc(scope, size),             \
            VALID, 0, scope                       \
    }

#define DECLARE_LIST_TYPE(type, ListType, new_list_func_name)         \
    typedef List(type) ListType;                                      \
                                                                      \
    inline static ListType new_list_func_name(Scope *scope, int size) \
    {                                                                 \
        return (ListType)_new_list(type, scope, size);                \
    }

#define _ensure_list_cap(lst, cap)                         \
    {                                                      \
        if (_list_cap(lst) < cap)                          \
        {                                                  \
            int newCap = 0;                                \
                                                           \
            _list_items(lst) =                             \
                ensure_capacity((lst).scope,               \
                                _list_items(lst),          \
                                sizeof(*_list_items(lst)), \
                                _list_size(lst),           \
                                _list_cap(lst),            \
                                cap,                       \
                                &newCap);                  \
                                                           \
            _list_cap(lst) = newCap;                       \
        }                                                  \
    }

#define push(lst, item)                                                              \
    {                                                                                \
        if ((lst)._state != VALID || (lst).scope->state != VALID || (lst)._readonly) \
        {                                                                            \
            QUIT(ERR_PUSH, __FILE__, __LINE__);                                      \
        }                                                                            \
                                                                                     \
        _ensure_list_cap(lst, 1 + _list_size(lst));                                  \
        _list_items(lst)[_list_size(lst)++] = (item);                                \
    }

#define resize_list(lst, sz)                                                         \
    {                                                                                \
        if ((lst)._state != VALID || (lst).scope->state != VALID || (lst)._readonly) \
        {                                                                            \
            QUIT(ERR_RESIZE, __FILE__, __LINE__);                                    \
        }                                                                            \
                                                                                     \
        _ensure_list_cap(lst, sz);                                                   \
        _list_size(lst) = sz;                                                        \
    }

#define pop(lst) ( \
    *(             \
        (0 < _list_size(lst) && (lst).scope->state == VALID && !(lst)._readonly) ? (_list_items(lst) + (--_list_size(lst))) : (typeof(_list_items(lst)))QUIT(ERR_POP, __FILE__, __LINE__).ptr))

#define at(lst, i) ( \
    *(               \
        ((unsigned)(i) < (unsigned)_list_size(lst) && (lst).scope->state == VALID) ? (_list_items(lst) + (i)) : (typeof(_list_items(lst)))QUIT(ERR_AT, __FILE__, __LINE__).ptr))

#define remove_list_at(lst, index)                                                                                                     \
    {                                                                                                                                  \
        if ((unsigned)(index) >= (unsigned)_list_size(lst) || (lst)._state != VALID || (lst).scope->state != VALID || (lst)._readonly) \
        {                                                                                                                              \
            QUIT(ERR_REMOVE_AT, __FILE__, __LINE__);                                                                                   \
        }                                                                                                                              \
                                                                                                                                       \
        for (int i = index; i < _list_size(lst) - 1; i++)                                                                              \
        {                                                                                                                              \
            _list_items(lst)[i] = _list_items(lst)[i + 1];                                                                             \
        }                                                                                                                              \
        _list_size(lst) -= 1;                                                                                                          \
    }

#if ALWAYS_BOUND_CHECK
#define at_q(lst, i) at(lst, i)
#else
#define at_q(lst, i) _list_items(lst)[i]
#endif

#define is_empty_list(lst) (_list_size(lst) == 0)

#define clear_list(lst) resize_list(lst, 0)

#define size_of_list(lst) ( \
    ((lst)._state == VALID && (lst).scope->state == VALID) ? _list_size(lst) : QUIT(ERR_LIST_SIZE, __FILE__, __LINE__).value)

#define list_data_ptr(lst) (*(lst)._p_items)

#define _check_range(lst, start, end) (start >= 0 && start <= end && end <= size_of_list(lst))

#define list_view(lst, start, end)                                                                                                                                                  \
    (typeof(lst))                                                                                                                                                                   \
    {                                                                                                                                                                               \
        _check_range(lst, start, end) ? (typeof((lst)._p_items))p_ref_alloc(scope, _list_items(lst) + start) : (typeof((lst)._p_items))QUIT(ERR_LIST_VIEW, __FILE__, __LINE__).ptr, \
            p_int_alloc(scope, end - start),                                                                                                                                        \
            p_int_alloc(scope, end - start),                                                                                                                                        \
            VALID, 1, scope                                                                                                                                                         \
    }

#define list_copy(dst, dst_start, src, src_start, src_end)                    \
    {                                                                         \
        int src_size = size_of_list(src);                                     \
        int dst_size = size_of_list(dst);                                     \
        if (src_start < 0 || src_end < src_start || src_end > src_size)       \
        {                                                                     \
            QUIT(ERR_SOURCE_INDEX, __FILE__, __LINE__);                       \
        }                                                                     \
                                                                              \
        int len = src_end - src_start;                                        \
        if (dst_start < 0 || dst_start + len > dst_size)                      \
        {                                                                     \
            QUIT(ERR_DEST_INDEX, __FILE__, __LINE__);                         \
        }                                                                     \
                                                                              \
        if (sizeof(*_list_items(src)) != sizeof(*_list_items(dst)))           \
        {                                                                     \
            QUIT(ERR_SRC_DST_TYPE, __FILE__, __LINE__);                       \
        }                                                                     \
                                                                              \
        memcpy(_list_items(dst) + dst_start,                                  \
               _list_items(src) + src_start, len * sizeof(_list_items(src))); \
    }

#define get_list_sum(lst, p_sum_var)                \
    {                                               \
        *(p_sum_var) = 0;                           \
        for (int i = 0; i < size_of_list(lst); i++) \
            *(p_sum_var) += _list_items(lst)[i];    \
    }

#define fprint_list(f, lst, fmt)                   \
    {                                              \
        typeof(lst) _lst = lst;                    \
        int len = size_of_list(_lst);              \
        fprintf(f, "[");                           \
                                                   \
        for (int i = 0; i < len; i++)              \
        {                                          \
            fprintf(f, fmt, _list_items(_lst)[i]); \
            if (i < len - 1)                       \
                fprintf(f, ", ");                  \
        }                                          \
                                                   \
        fprintf(f, "]\n");                         \
    }

#define fprint_list_obj(f, lst, fprint_item)      \
    {                                             \
        typeof(lst) _lst = lst;                   \
        int len = size_of_list(_lst);             \
        fprintf(f, "[");                          \
                                                  \
        for (int i = 0; i < len; i++)             \
        {                                         \
            fprint_item(f, _list_items(_lst)[i]); \
            if (i < len - 1)                      \
                fprintf(f, ", ");                 \
        }                                         \
                                                  \
        fprintf(f, "]\n");                        \
    }

DECLARE_LIST_TYPE(int, ListInt, new_list_int);
DECLARE_LIST_TYPE(char, ListChar, new_list_char);
DECLARE_LIST_TYPE(long, ListLong, new_list_long);
DECLARE_LIST_TYPE(short, ListShort, new_list_short);
DECLARE_LIST_TYPE(double, ListDouble, new_list_double);
DECLARE_LIST_TYPE(float, ListFloat, new_list_float);

#endif