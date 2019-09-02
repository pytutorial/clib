#ifndef __MAP__H_
#define __MAP__H_
#include "list.h"
#include "object.h"

#define ERR_MAP "Access an invalid map"
#define MAP_BUCKET_SIZE 1024

#define _ItemType(K2V) Item##K2V
#define _MapTypeData(K2V) _Map##K2V##Data
#define MapType(K2V) Map##K2V
#define _ListItemType(K2V) ListItem##K2V
#define _ListListItemType(K2V) ListListItem##K2V
#define _newListItemFunc(K2V) newListItem##K2V
#define _newListListItemFunc(K2V) newListListItem##K2V
#define _mapGetFunc(K2V) _map##K2V##Get
#define _mapGetErrFunc(K2V) _map##K2V##GetErr
#define _mapPutFunc(K2V) _map##K2V##Put
#define _mapContainsKeyFunc(K2V) _map##K2V##Contains
#define _mapRemoveFunc(K2V) _map##K2V##Remove
#define _mapItemsFunc(K2V) _map##K2V##Items
#define _mapItemsErrFunc(K2V) _map##K2V##ItemsErr
#define _printMapFunc(K2V) _printMap##K2V
#define _newMapFunc(K2V) newMap##K2V

#define _DECLARE_MAP_TYPE(K2V, K, V)                                                                                    \
    typedef struct                                                                                                      \
    {                                                                                                                   \
        K key;                                                                                                          \
        V value;                                                                                                        \
    } _ItemType(K2V);                                                                                                   \
    DECLARE_LIST(_ListItemType(K2V), _ItemType(K2V));                                                                   \
    DECLARE_LIST(_ListListItemType(K2V), _ListItemType(K2V));                                                           \
    typedef struct _MapTypeData(K2V)                                                                                    \
    {                                                                                                                   \
        _ListListItemType(K2V) _table;                                                                                  \
        int _size;                                                                                                      \
        int _n_active_bucket;                                                                                           \
        int (*_hash_func)(K);                                                                                           \
        bool (*_equal_func)(K, K);                                                                                      \
        V (*_get)                                                                                                       \
        (struct _MapTypeData(K2V) * m, K key, V default_value);                                                         \
        V (*_get_err)                                                                                                   \
        (V default_value);                                                                                              \
        void (*_put)(struct _MapTypeData(K2V) * m, K key, V value);                                                     \
        bool (*_contains_key)(struct _MapTypeData(K2V) * m, K key);                                                     \
        void (*_remove)(struct _MapTypeData(K2V) * m, K key);                                                           \
        _ListItemType(K2V) (*_items)(struct _MapTypeData(K2V) * m);                                                     \
        _ListItemType(K2V) (*_items_err)(struct _MapTypeData(K2V) * m);                                                 \
        void (*_print)(struct _MapTypeData(K2V) * m, void (*print_key_func)(K key), void (*print_value_func)(V value)); \
        Scope scope;                                                                                                    \
    }                                                                                                                   \
    *MapType(K2V);

#define _DECLARE_MAP_GET(K2V, K, V)                                          \
    inline static V _mapGetFunc(K2V)(MapType(K2V) m, K key, V default_value) \
    {                                                                        \
        int hash = (m->_hash_func)(key);                                     \
        int index = (unsigned)hash % m->_table->_size;                       \
        _ListItemType(K2V) lst_item = list_at_q(m->_table, index);           \
        for (int i = 0; i < lst_item->_size; i++)                            \
        {                                                                    \
            if ((m->_equal_func)(key, list_at_q(lst_item, i).key))           \
            {                                                                \
                return list_at_q(lst_item, i).value;                         \
            }                                                                \
        }                                                                    \
        return default_value;                                                \
    }

#define _DECLARE_MAP_GET_ERR(K2V, K, V)                  \
    inline static V _mapGetErrFunc(K2V)(V default_value) \
    {                                                    \
        QUIT(ERR_MAP, __FILE__, __LINE__);               \
        return default_value;                            \
    }

#define _DECLARE_MAP_PUT(K2V, K, V)                                     \
    inline static void _mapPutFunc(K2V)(MapType(K2V) m, K key, V value) \
    {                                                                   \
        int hash = (m->_hash_func)(key);                                \
        int index = (unsigned)hash % m->_table->_size;                  \
        _ListItemType(K2V) lst_item = list_at_q(m->_table, index);      \
                                                                        \
        if (lst_item->_size == 0)                                       \
        {                                                               \
            int n_active_bucket = ++(m->_n_active_bucket);              \
                                                                        \
            if (n_active_bucket > m->_table->_size / 2)                 \
            {                                                           \
                list_resize(m->_table, 3 * m->_table->_size / 2);       \
            }                                                           \
        }                                                               \
                                                                        \
        int i = 0;                                                      \
        for (i = 0; i < lst_item->_size; i++)                           \
        {                                                               \
            if ((m->_equal_func)(key, list_at_q(lst_item, i).key))      \
                break;                                                  \
        }                                                               \
        if (i < lst_item->_size)                                        \
        {                                                               \
            list_at_q(lst_item, i).value = value;                       \
        }                                                               \
        else                                                            \
        {                                                               \
            list_add(lst_item, ((_ItemType(K2V)){key, value}));         \
            m->_size += 1;                                              \
        }                                                               \
    }

#define _DECLARE_MAP_CONTAINS(K2V, K, V)                               \
    inline static bool _mapContainsKeyFunc(K2V)(MapType(K2V) m, K key) \
    {                                                                  \
        int hash = (m->_hash_func)(key);                               \
        int index = (unsigned)hash % m->_table->_size;                 \
        _ListItemType(K2V) lst_item = list_at_q(m->_table, index);     \
        for (int i = 0; i < lst_item->_size; i++)                      \
        {                                                              \
            if ((m->_equal_func)(key, list_at_q(lst_item, i).key))     \
            {                                                          \
                return TRUE;                                           \
            }                                                          \
        }                                                              \
        return FALSE;                                                  \
    }

#define _DECLARE_MAP_REMOVE(K2V, K, V)                             \
    inline static void _mapRemoveFunc(K2V)(MapType(K2V) m, K key)  \
    {                                                              \
        int hash = (m->_hash_func)(key);                           \
        int index = (unsigned)hash % m->_table->_size;             \
        _ListItemType(K2V) lst_item = list_at_q(m->_table, index); \
        for (int i = 0; i < lst_item->_size; i++)                  \
        {                                                          \
            if ((m->_equal_func)(key, list_at_q(lst_item, i).key)) \
            {                                                      \
                list_remove_at(lst_item, i);                       \
                return;                                            \
            }                                                      \
        }                                                          \
    }

#define _DECLARE_MAP_ITEMS(K2V, K, V)                                   \
    inline static _ListItemType(K2V) _mapItemsFunc(K2V)(MapType(K2V) m) \
    {                                                                   \
        _ListItemType(K2V) items = _newListItemFunc(K2V)(m->scope, 0);  \
        for (int index = 0; index < m->_table->_size; index++)          \
        {                                                               \
            _ListItemType(K2V) lst_item = list_at_q(m->_table, index);  \
            for (int i = 0; i < lst_item->_size; i++)                   \
            {                                                           \
                list_add(items, list_at_q(lst_item, i));                \
            }                                                           \
        }                                                               \
        return items;                                                   \
    }

#define _DECLARE_MAP_ITEMS_ERR(K2V, K, V)                                  \
    inline static _ListItemType(K2V) _mapItemsErrFunc(K2V)(MapType(K2V) m) \
    {                                                                      \
        QUIT(ERR_MAP, __FILE__, __LINE__);                                 \
        _ListItemType(K2V) items = _newListItemFunc(K2V)(m->scope, 0);     \
        return items;                                                      \
    }

#define _DECLARE_MAP_PRINT(K2V, K, V)                                                                                       \
    inline static void _printMapFunc(K2V)(MapType(K2V) m, void (*print_key_func)(K key), void (*print_value_func)(V value)) \
    {                                                                                                                       \
        _ListItemType(K2V) items = _mapItemsFunc(K2V)(m);                                                                   \
        printf("{");                                                                                                        \
        for (int i = 0; i < items->_size; i++)                                                                              \
        {                                                                                                                   \
            print_key_func(list_at_q(items, i).key);                                                                        \
            printf(" : ");                                                                                                  \
            print_value_func(list_at_q(items, i).value);                                                                    \
            if (i < items->_size - 1)                                                                                       \
                printf(" , ");                                                                                              \
        }                                                                                                                   \
        printf("}\n");                                                                                                      \
    }

#define _DECLARE_MAP_NEW(K2V, K, V, hash_func, equal_func)                                     \
    inline static MapType(K2V) _newMapFunc(K2V)(Scope scope)                                   \
    {                                                                                          \
        MapType(K2V) m = zero_alloc(scope, sizeof(struct _MapTypeData(K2V)));                  \
        m->_table = (_ListListItemType(K2V))_newListListItemFunc(K2V)(scope, MAP_BUCKET_SIZE); \
        m->_size = 0;                                                                          \
        m->_n_active_bucket = 0;                                                               \
        m->_hash_func = hash_func;                                                             \
        m->_equal_func = equal_func;                                                           \
        m->_get = _mapGetFunc(K2V);                                                            \
        m->_get_err = _mapGetErrFunc(K2V);                                                     \
        m->_put = _mapPutFunc(K2V);                                                            \
        m->_contains_key = _mapContainsKeyFunc(K2V);                                           \
        m->_remove = _mapRemoveFunc(K2V);                                                      \
        m->_items = _mapItemsFunc(K2V);                                                        \
        m->_items_err = _mapItemsErrFunc(K2V);                                                 \
        m->_print = _printMapFunc(K2V);                                                        \
        m->scope = scope;                                                                      \
                                                                                               \
        for (int i = 0; i < MAP_BUCKET_SIZE; i++)                                              \
        {                                                                                      \
            list_at_q(m->_table, i) = (_ListItemType(K2V))_newListItemFunc(K2V)(scope, 0);     \
        }                                                                                      \
        return m;                                                                              \
    }

#define DECLARE_MAP(K2V, K, V, hash_func, equal_func) \
    _DECLARE_MAP_TYPE(K2V, K, V)                      \
    _DECLARE_MAP_GET(K2V, K, V)                       \
    _DECLARE_MAP_GET_ERR(K2V, K, V)                   \
    _DECLARE_MAP_PUT(K2V, K, V)                       \
    _DECLARE_MAP_CONTAINS(K2V, K, V)                  \
    _DECLARE_MAP_REMOVE(K2V, K, V)                    \
    _DECLARE_MAP_ITEMS(K2V, K, V)                     \
    _DECLARE_MAP_ITEMS_ERR(K2V, K, V)                 \
    _DECLARE_MAP_PRINT(K2V, K, V)                     \
    _DECLARE_MAP_NEW(K2V, K, V, hash_func, equal_func)

#define _is_valid_map(m) ((m) != NULL && (m)->scope != NULL && (m)->scope->state == VALID)
#define _check_map_valid(m)                \
    if (!_is_valid_map(m))                 \
    {                                      \
        QUIT(ERR_MAP, __FILE__, __LINE__); \
    }

#define map_size(m) (_is_valid_map(m) ? (m)->_size : QUIT(ERR_MAP, __FILE__, __LINE__).value)
#define map_is_empty(m) (map_size(m) == 0)

#define map_get(m, key, default_value) (_is_valid_map(m) ? ((m)->_get)(m, key, default_value) : (m)->_get_err(default_value))
#define map_contains_key(m, key) (_is_valid_map(m) ? ((m)->_contains_key)(m, key) : QUIT(ERR_MAP, __FILE__, __LINE__).value)
#define map_items(m) (_is_valid_map(m) ? ((m)->_items)(m) : (m)->_items_err(m))

#define map_put(m, key, value)      \
    {                               \
        _check_map_valid(m);        \
        ((m)->_put)(m, key, value); \
    }
#define map_remove(m, key)      \
    {                           \
        _check_map_valid(m);    \
        ((m)->_remove)(m, key); \
    }

#define print_map(m, print_key_func, print_value_func)          \
    {                                                           \
        if (_is_valid_map(m))                                   \
        {                                                       \
            ((m)->_print)(m, print_key_func, print_value_func); \
        }                                                       \
    }

static inline int hash_int(int x) { return x; }
static inline bool int_equal(int x1, int x2) { return x1 == x2; }

DECLARE_MAP(Int2Int, int, int, hash_int, int_equal);

#endif