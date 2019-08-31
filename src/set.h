#ifndef __SET__H_
#define __SET__H_
#include "list.h"
#include "object.h"
#include "string_util.h"

#define ERR_SET     "Add to an invalid set"

#define BUCKET_SIZE 1024

#define Set(T)                   \
    struct                       \
    {                            \
        List(List(T)) _table;    \
        int *_p_size;            \
        int *_p_active_bucket;   \
        int _state;              \
        int (*_hash_func)(T);    \
        bool (*_cmp_func)(T, T); \
        Scope *scope;            \
    }

#define _item_type(s) typeof(at(at_q((s)._table, 0), 0))

#define DECLARE_SET_TYPE(SetType, T, hash_func, cmp_func, new_set_func_name)                   \
    typedef Set(T) SetType;                                                                    \
                                                                                               \
    SetType new_set_func_name(Scope *scope)                                                    \
    {                                                                                          \
        SetType s;                                                                             \
        s._table = (typeof(s._table))_new_list(typeof(at_q(s._table, 0)), scope, BUCKET_SIZE); \
        s._p_size = p_int_alloc(scope, 0);                                                     \
        s._p_active_bucket = p_int_alloc(scope, 0);                                            \
        s._state = VALID;                                                                      \
        s._hash_func = hash_func;                                                              \
        s._cmp_func = cmp_func;                                                                \
        s.scope = scope;                                                                       \
                                                                                               \
        for (int i = 0; i < BUCKET_SIZE; i++)                                                  \
        {                                                                                      \
            at_q(s._table, i) = (typeof(at_q(s._table, i)))_new_list(_item_type(s), scope, 0); \
        }                                                                                      \
        return s;                                                                              \
    }

#define size_of_set(s) ( \
    ((s)._state == VALID && (s).scope->state == VALID) ? *(s)._p_size : QUIT(ERR_SET, __FILE__, __LINE__).value)

#define is_empty_set(s) (size_of_set(s) == 0)

#define add_to_set(s, item)                                              \
    {                                                                    \
        if ((s)._state != VALID || (s).scope->state != VALID)            \
        {                                                                \
            QUIT(ERR_SET, __FILE__, __LINE__);                           \
        }                                                                \
                                                                         \
        int hash = ((s)._hash_func)(item);                               \
        int index = (unsigned)hash % _list_size((s)._table);             \
        typeof(at_q((s)._table, 0)) lst_items = at_q((s)._table, index); \
                                                                         \
        if (_list_size(lst_items) == 0)                                  \
        {                                                                \
            int n_bucket = _list_size((s)._table);                       \
            int active_bucket = ++(*(s)._p_active_bucket);               \
                                                                         \
            if (active_bucket > n_bucket / 2)                            \
            {                                                            \
                resize_list((s)._table, 3 * n_bucket / 2);               \
            }                                                            \
        }                                                                \
                                                                         \
        bool contained = FALSE;                                          \
        for (int i = 0; i < _list_size(lst_items); i++)                  \
        {                                                                \
            if (((s)._cmp_func)(item, at_q(lst_items, i)))               \
            {                                                            \
                contained = TRUE;                                        \
                break;                                                   \
            }                                                            \
        }                                                                \
        if (!contained)                                                  \
        {                                                                \
            push(lst_items, item);                                       \
            *(s)._p_size += 1;                                           \
        }                                                                \
    }

#define check_set_contain(s, item, p_is_contained)                       \
    {                                                                    \
        if ((s)._state != VALID || (s).scope->state != VALID)            \
        {                                                                \
            QUIT(ERR_SET, __FILE__, __LINE__);                           \
        }                                                                \
                                                                         \
        int hash = ((s)._hash_func)(item);                               \
        int index = (unsigned)hash % _list_size((s)._table);             \
        typeof(at_q((s)._table, 0)) lst_items = at_q((s)._table, index); \
        *(p_is_contained) = FALSE;                                       \
                                                                         \
        for (int i = 0; i < _list_size(lst_items); i++)                  \
        {                                                                \
            if (((s)._cmp_func)(item, at_q(lst_items, i)))               \
            {                                                            \
                *(p_is_contained) = TRUE;                                \
                break;                                                   \
            }                                                            \
        }                                                                \
    }

#define remove_set_item(s, item)                                         \
    {                                                                    \
        if ((s)._state != VALID || (s).scope->state != VALID)            \
        {                                                                \
            QUIT(ERR_SET, __FILE__, __LINE__);                           \
        }                                                                \
        int hash = ((s)._hash_func)(item);                               \
        int index = (unsigned)hash % _list_size((s)._table);             \
        typeof(at_q((s)._table, 0)) lst_items = at_q((s)._table, index); \
                                                                         \
        for (int i = 0; i < _list_size(lst_items); i++)                  \
        {                                                                \
            if (((s)._cmp_func)(item, at_q(lst_items, i)))               \
            {                                                            \
                remove_list_at(lst_items, i);                            \
                break;                                                   \
            }                                                            \
        }                                                                \
    }

#define get_set_items(s, lst)                                            \
    {                                                                    \
        if ((s)._state != VALID || (s).scope->state != VALID)            \
        {                                                                \
            QUIT(ERR_SET, __FILE__, __LINE__);                           \
        }                                                                \
                                                                         \
        for (int i = 0; i < _list_size((s)._table); i++)                 \
        {                                                                \
            typeof(at_q((s)._table, 0)) lst_items = at_q((s)._table, i); \
            for (int j = 0; j < _list_size(lst_items); j++)              \
            {                                                            \
                push(lst, at_q(lst_items, j));                           \
            }                                                            \
        }                                                                \
    }

static inline int hash_int(int x) { return x; }
static inline bool cmp_int(int x1, int x2) { return x1 == x2; }

DECLARE_SET_TYPE(SetInt, int, hash_int, cmp_int, new_set_int);
DECLARE_SET_TYPE(SetString, String, hash_string, string_equal, new_set_string);

#endif