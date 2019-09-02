#ifndef __set__H_
#define __set__H_
#include "list.h"
#include "object.h"

#define ERR_SET                                 "Access an invalid set"
#define SET_BUCKET_SIZE                         1024

#define _SetTypeData(SetType)                   _##SetType##Data
#define _ListItem(SetType)                      List##SetType##Item
#define _ListListItem(SetType)                  ListList##SetType##Item
#define _newListItemFunc(SetType)               newList##SetType##Item
#define _newListListItemFunc(SetType)           newListList##SetType##Item
#define _setAddFunc(SetType)                    _##SetType##Add
#define _setContainsFunc(SetType)               _##SetType##Contains
#define _setRemoveFunc(SetType)                 _##SetType##Remove
#define _setItemsFunc(SetType)                  _##SetType##Items
#define _setItemsErrFunc(SetType)               _##SetType##ItemsErr
#define _printSetFunc(SetType)                  _print##SetType
#define _newSetFunc(SetType)                    new##SetType

#define _DECLARE_SET_TYPE(SetType, T)                                                      \
    DECLARE_LIST(_ListItem(SetType), T);                                                   \
    DECLARE_LIST(_ListListItem(SetType), _ListItem(SetType));                              \
    typedef struct _SetTypeData(SetType)                                                   \
    {                                                                                      \
        _ListListItem(SetType) _table;                                                     \
        int _size;                                                                         \
        int _n_active_bucket;                                                              \
        int (*_hash_func)(T);                                                              \
        bool (*_equal_func)(T, T);                                                         \
        void (*_add)(struct _SetTypeData(SetType) * s, T item);                            \
        bool (*_contains)(struct _SetTypeData(SetType) * s, T item);                       \
        void (*_remove)(struct _SetTypeData(SetType) * s, T item);                         \
        _ListItem(SetType) (*_items)(struct _SetTypeData(SetType) * m);                    \
        _ListItem(SetType) (*_items_err)(struct _SetTypeData(SetType) * m);                \
        void (*_print)(struct _SetTypeData(SetType) * s, void (*print_item_func)(T item)); \
        Scope scope;                                                                       \
    }                                                                                      \
    *SetType;

#define _DECLARE_SET_ADD(SetType, T)                               \
    inline static void _setAddFunc(SetType)(SetType s, T item)     \
    {                                                              \
        int hash = (s->_hash_func)(item);                          \
        int index = (unsigned)hash % s->_table->_size;             \
        _ListItem(SetType) lst_item = list_at_q(s->_table, index); \
                                                                   \
        if (lst_item->_size == 0)                                  \
        {                                                          \
            int n_active_bucket = ++(s->_n_active_bucket);         \
                                                                   \
            if (n_active_bucket > s->_table->_size / 2)            \
            {                                                      \
                list_resize(s->_table, 3 * s->_table->_size / 2);  \
            }                                                      \
        }                                                          \
        bool contained = FALSE;                                    \
        for (int i = 0; i < lst_item->_size; i++)                  \
        {                                                          \
            if (((s)->_equal_func)(item, list_at_q(lst_item, i)))  \
            {                                                      \
                contained = TRUE;                                  \
                break;                                             \
            }                                                      \
        }                                                          \
        if (!contained)                                            \
        {                                                          \
            list_add(lst_item, item);                              \
            s->_size += 1;                                         \
        }                                                          \
    }

#define _DECLARE_SET_CONTAINS(SetType, T)                           \
    inline static bool _setContainsFunc(SetType)(SetType s, T item) \
    {                                                               \
        int hash = (s->_hash_func)(item);                           \
        int index = (unsigned)hash % s->_table->_size;              \
        _ListItem(SetType) lst_item = list_at_q(s->_table, index);  \
        for (int i = 0; i < lst_item->_size; i++)                   \
        {                                                           \
            if ((s->_equal_func)(item, list_at_q(lst_item, i)))     \
            {                                                       \
                return TRUE;                                        \
            }                                                       \
        }                                                           \
        return FALSE;                                               \
    }

#define _DECLARE_SET_REMOVE(SetType, T)                            \
    inline static void _setRemoveFunc(SetType)(SetType s, T item)  \
    {                                                              \
        int hash = (s->_hash_func)(item);                          \
        int index = (unsigned)hash % s->_table->_size;             \
        _ListItem(SetType) lst_item = list_at_q(s->_table, index); \
        for (int i = 0; i < lst_item->_size; i++)                  \
        {                                                          \
            if ((s->_equal_func)(item, list_at_q(lst_item, i)))    \
            {                                                      \
                list_remove_at(lst_item, i);                       \
                return;                                            \
            }                                                      \
        }                                                          \
    }

#define _DECLARE_SET_ITEMS(SetType, T)                                     \
    inline static _ListItem(SetType) _setItemsFunc(SetType)(SetType s)     \
    {                                                                      \
        _ListItem(SetType) items = _newListItemFunc(SetType)(s->scope, 0); \
        for (int index = 0; index < s->_table->_size; index++)             \
        {                                                                  \
            _ListItem(SetType) lst_item = list_at_q(s->_table, index);     \
            for (int i = 0; i < lst_item->_size; i++)                      \
            {                                                              \
                list_add(items, list_at_q(lst_item, i));                   \
            }                                                              \
        }                                                                  \
        return items;                                                      \
    }

#define _DECLARE_SET_ITEMS_ERR(SetType, T)                                 \
    inline static _ListItem(SetType) _setItemsErrFunc(SetType)(SetType s)  \
    {                                                                      \
        QUIT(ERR_SET, __FILE__, __LINE__);                                 \
        _ListItem(SetType) items = _newListItemFunc(SetType)(s->scope, 0); \
        return items;                                                      \
    }

#define _DECLARE_SET_PRINT(SetType, T)                                                    \
    inline static void _printSetFunc(SetType)(SetType s, void (*print_item_func)(T item)) \
    {                                                                                     \
        _ListItem(SetType) items = _setItemsFunc(SetType)(s);                             \
        printf("{");                                                                      \
        for (int i = 0; i < items->_size; i++)                                            \
        {                                                                                 \
            print_item_func(list_at_q(items, i));                                         \
            if (i < items->_size - 1)                                                     \
                printf(" , ");                                                            \
        }                                                                                 \
        printf("}\n");                                                                    \
    }

#define _DECLARE_SET_NEW(SetType, T, hash_func, equal_func)                                        \
    inline static SetType _newSetFunc(SetType)(Scope scope)                                        \
    {                                                                                              \
        SetType s = zero_alloc(scope, sizeof(struct _SetTypeData(SetType)));                       \
        s->_table = (_ListListItem(SetType))_newListListItemFunc(SetType)(scope, SET_BUCKET_SIZE); \
        s->_size = 0;                                                                              \
        s->_n_active_bucket = 0;                                                                   \
        s->_hash_func = hash_func;                                                                 \
        s->_equal_func = equal_func;                                                               \
        s->_add = _setAddFunc(SetType);                                                            \
        s->_contains = _setContainsFunc(SetType);                                                  \
        s->_remove = _setRemoveFunc(SetType);                                                      \
        s->_items = _setItemsFunc(SetType);                                                        \
        s->_items_err = _setItemsErrFunc(SetType);                                                 \
        s->_print = _printSetFunc(SetType);                                                        \
        s->scope = scope;                                                                          \
                                                                                                   \
        for (int i = 0; i < SET_BUCKET_SIZE; i++)                                                  \
        {                                                                                          \
            list_at_q(s->_table, i) = (_ListItem(SetType))_newListItemFunc(SetType)(scope, 0);     \
        }                                                                                          \
        return s;                                                                                  \
    }

#define DECLARE_SET(SetType, T, hash_func, equal_func) \
    _DECLARE_SET_TYPE(SetType, T)                      \
    _DECLARE_SET_ADD(SetType, T)                       \
    _DECLARE_SET_CONTAINS(SetType, T)                  \
    _DECLARE_SET_REMOVE(SetType, T)                    \
    _DECLARE_SET_ITEMS(SetType, T)                     \
    _DECLARE_SET_ITEMS_ERR(SetType, T)                 \
    _DECLARE_SET_PRINT(SetType, T)                     \
    _DECLARE_SET_NEW(SetType, T, hash_func, equal_func)

#define _is_valid_set(s) ((s) != NULL && (s)->scope != NULL && (s)->scope->state == VALID)

#define _check_set_valid(s)                \
    if (!_is_valid_set(s))                 \
    {                                      \
        QUIT(ERR_SET, __FILE__, __LINE__); \
    }

#define set_size(s) (_is_valid_set(s) ? (s)->_size : QUIT(ERR_SET, __FILE__, __LINE__).value)
#define set_is_empty(s) (set_size(s) == 0)

#define set_contains(s, item) (_is_valid_set(s) ? ((s)->_contains)(s, item) : QUIT(ERR_SET, __FILE__, __LINE__).value)
#define set_items(s) (_is_valid_set(s) ? ((s)->_items)(s) : (s)->_items_err(s))

#define set_add(s, item)      \
    {                         \
        _check_set_valid(s);  \
        ((s)->_add)(s, item); \
    }

#define set_remove(s, item)      \
    {                            \
        _check_set_valid(s);     \
        ((s)->_remove)(s, item); \
    }

#define print_set(s, print_item_func)          \
    {                                          \
        if (_is_valid_set(s))                  \
        {                                      \
            ((s)->_print)(s, print_item_func); \
        }                                      \
    }

static inline int hash_int(int x) { return x; }
static inline bool int_equal(int x1, int x2) { return x1 == x2; }

static inline int hash_long(long x) { return x; }
static inline bool long_equal(long x1, long x2) { return x1 == x2; }

DECLARE_SET(SetInt, int, hash_int, int_equal);
DECLARE_SET(SetLong, long, hash_long, long_equal);

#endif