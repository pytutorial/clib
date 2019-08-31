#ifndef __MAP__H_
#define __MAP__H_
#include "list.h"
#include "object.h"
#include "string_util.h"

#define ERR_MAP_PUT "Put to an invalid map"
#define ERR_MAP_GET "Get value of an invalid map"
#define ERR_MAP_GET_ITEMS "Get items of an invalid map"

#define BUCKET_SIZE 1024

#define Map(K, V)                      \
    struct                             \
    {                                  \
        List(List(struct {K key; V value; })) _table; \
        int *_p_size;                  \
        int *_p_active_bucket;         \
        int _state;                    \
        int (*_hash_func)(K);          \
        bool (*_cmp_func)(K, K);       \
        Scope *scope;                  \
    }

#define _item_type(m) typeof(at(at_q((m)._table, 0), 0))

#define DECLARE_MAP_TYPE(MapType, K, V, hash_func, cmp_func, new_map_func_name)                \
    typedef Map(K, V) MapType;                                                                 \
                                                                                               \
    MapType new_map_func_name(Scope *scope)                                                    \
    {                                                                                          \
        MapType m;                                                                             \
        m._table = (typeof(m._table))_new_list(typeof(at_q(m._table, 0)), scope, BUCKET_SIZE); \
        m._p_size = p_int_alloc(scope, 0);                                                     \
        m._p_active_bucket = p_int_alloc(scope, 0);                                            \
        m._state = VALID;                                                                      \
        m._hash_func = hash_func;                                                              \
        m._cmp_func = cmp_func;                                                                \
        m.scope = scope;                                                                       \
                                                                                               \
        for (int i = 0; i < BUCKET_SIZE; i++)                                                  \
        {                                                                                      \
            at_q(m._table, i) = (typeof(at_q(m._table, i)))_new_list(_item_type(m), scope, 0); \
        }                                                                                      \
        return m;                                                                              \
    }

#define map_size(m) ( \
    ((m)._state == VALID && (m).scope->state == VALID) ? *(m)._p_size : QUIT(ERR_MAP_SIZE, __FILE__, __LINE__).value)

#define map_put(m, key, value)                                \
    {                                                         \
        if ((m)._state != VALID || (m).scope->state != VALID) \
        {                                                     \
            QUIT(ERR_MAP_PUT, __FILE__, __LINE__);            \
        }                                                     \
                                                              \
        int hash = (m._hash_func)(key);                       \
        int index = (unsigned)hash % _list_size(m._table);    \
                                                              \
        if (_list_size(at_q(m._table, index)) == 0)           \
        {                                                     \
            int n_bucket = _list_size((m)._table);            \
            int active_bucket = ++(*(m)._p_active_bucket);    \
                                                              \
            if (active_bucket > n_bucket / 2)                 \
            {                                                 \
                resize_list((m)._table, 3 * n_bucket / 2);    \
            }                                                 \
        }                                                     \
        _item_type(m) new_item = {key, value};                \
        push(at_q(m._table, index), new_item);                \
        *(m)._p_size += 1;                                    \
    }

#define map_get(m, _key, p_value, default_value)                     \
    {                                                                \
        if ((m)._state != VALID || (m).scope->state != VALID)        \
        {                                                            \
            QUIT(ERR_MAP_GET, __FILE__, __LINE__);                   \
        }                                                            \
                                                                     \
        int hash = (m._hash_func)(_key);                             \
        int index = (unsigned)hash % _list_size(m._table);           \
        typeof(at_q(m._table, 0)) lst_items = at_q(m._table, index); \
        bool found = FALSE;                                          \
        for (int i = 0; i < _list_size(lst_items); i++)              \
        {                                                            \
            if ((m._cmp_func)(_key, at_q(lst_items, i).key))         \
            {                                                        \
                *(p_value) = at_q(lst_items, i).value;               \
                found = TRUE;                                        \
                break;                                               \
            }                                                        \
        }                                                            \
        if (!found)                                                  \
            *(p_value) = default_value;                              \
    }

#define map_get_items(m, keys, values)                                   \
    {                                                                    \
        if ((m)._state != VALID || (m).scope->state != VALID)            \
        {                                                                \
            QUIT(ERR_MAP_GET_ITEMS, __FILE__, __LINE__);                 \
        }                                                                \
                                                                         \
        for (int i = 0; i < _list_size((m)._table); i++)                 \
        {                                                                \
            typeof(at_q((m)._table, 0)) lst_items = at_q((m)._table, i); \
            for (int j = 0; j < _list_size(lst_items); j++)              \
            {                                                            \
                push(keys, at_q(lst_items, j).key);                      \
                push(values, at_q(lst_items, j).value);                  \
            }                                                            \
        }                                                                \
    }

static inline int hash_int(int x) { return x; }
static inline bool cmp_int(int x1, int x2) { return x1 == x2; }

static inline int hash_string(String st)
{
    int *p = (int *)(str_data(st));
    int hash = 0;
    for (int i = 0; i < str_len(st) / 4; i++)
        hash |= *p++;
    return hash;
}

DECLARE_MAP_TYPE(MapIntInt, int, int, hash_int, cmp_int, new_map_int_int);
DECLARE_MAP_TYPE(MapIntString, int, String, hash_int, cmp_int, new_map_int_string);
DECLARE_MAP_TYPE(MapStringInt, String, int, hash_string, string_equal, new_map_string_int);

#endif