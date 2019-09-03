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

#define _DECLARE_SET_TYPE(SetType, T)                                                    \
    DECLARE_LIST_TYPE(_ListItem(SetType), T);                                                 \
    DECLARE_LIST_TYPE(_ListListItem(SetType), _ListItem(SetType));                            \
    typedef struct _SetTypeData(SetType)                                                 \
    {                                                                                    \
        _ListListItem(SetType) _table;                                                   \
        int _size;                                                                       \
        int _nActiveBucket;                                                              \
        int (*_hashFunc)(T);                                                             \
        bool (*_equalFunc)(T, T);                                                        \
        void (*_add)(struct _SetTypeData(SetType) * s, T item);                          \
        bool (*_contains)(struct _SetTypeData(SetType) * s, T item);                     \
        void (*_remove)(struct _SetTypeData(SetType) * s, T item);                       \
        _ListItem(SetType) (*_items)(struct _SetTypeData(SetType) * m);                  \
        _ListItem(SetType) (*_itemsErr)(struct _SetTypeData(SetType) * m);               \
        void (*_print)(struct _SetTypeData(SetType) * s, void (*printItemFunc)(T item)); \
        Scope scope;                                                                     \
    }                                                                                    \
    *SetType;

#define _DECLARE_SET_TYPE_ADD(SetType, T)                        \
    inline static void _setAddFunc(SetType)(SetType s, T item)   \
    {                                                            \
        int hash = (s->_hashFunc)(item);                         \
        int index = (unsigned)hash % s->_table->_size;           \
        _ListItem(SetType) lstItem = listAtQ(s->_table, index);  \
                                                                 \
        if (lstItem->_size == 0)                                 \
        {                                                        \
            int nActiveBucket = ++(s->_nActiveBucket);           \
                                                                 \
            if (nActiveBucket > s->_table->_size / 2)            \
            {                                                    \
                listResize(s->_table, 3 * s->_table->_size / 2); \
            }                                                    \
        }                                                        \
        bool contained = FALSE;                                  \
        for (int i = 0; i < lstItem->_size; i++)                 \
        {                                                        \
            if (((s)->_equalFunc)(item, listAtQ(lstItem, i)))    \
            {                                                    \
                contained = TRUE;                                \
                break;                                           \
            }                                                    \
        }                                                        \
        if (!contained)                                          \
        {                                                        \
            listAdd(lstItem, item);                              \
            s->_size += 1;                                       \
        }                                                        \
    }

#define _DECLARE_SET_TYPE_CONTAINS(SetType, T)                      \
    inline static bool _setContainsFunc(SetType)(SetType s, T item) \
    {                                                               \
        int hash = (s->_hashFunc)(item);                            \
        int index = (unsigned)hash % s->_table->_size;              \
        _ListItem(SetType) lstItem = listAtQ(s->_table, index);     \
        for (int i = 0; i < lstItem->_size; i++)                    \
        {                                                           \
            if ((s->_equalFunc)(item, listAtQ(lstItem, i)))         \
            {                                                       \
                return TRUE;                                        \
            }                                                       \
        }                                                           \
        return FALSE;                                               \
    }

#define _DECLARE_SET_TYPE_REMOVE(SetType, T)                      \
    inline static void _setRemoveFunc(SetType)(SetType s, T item) \
    {                                                             \
        int hash = (s->_hashFunc)(item);                          \
        int index = (unsigned)hash % s->_table->_size;            \
        _ListItem(SetType) lstItem = listAtQ(s->_table, index);   \
        for (int i = 0; i < lstItem->_size; i++)                  \
        {                                                         \
            if ((s->_equalFunc)(item, listAtQ(lstItem, i)))       \
            {                                                     \
                listRemoveAt(lstItem, i);                         \
                return;                                           \
            }                                                     \
        }                                                         \
    }

#define _DECLARE_SET_TYPE_ITEMS(SetType, T)                                \
    inline static _ListItem(SetType) _setItemsFunc(SetType)(SetType s)     \
    {                                                                      \
        _ListItem(SetType) items = _newListItemFunc(SetType)(s->scope, 0); \
        for (int index = 0; index < s->_table->_size; index++)             \
        {                                                                  \
            _ListItem(SetType) lstItem = listAtQ(s->_table, index);        \
            for (int i = 0; i < lstItem->_size; i++)                       \
            {                                                              \
                listAdd(items, listAtQ(lstItem, i));                       \
            }                                                              \
        }                                                                  \
        return items;                                                      \
    }

#define _DECLARE_SET_TYPE_ITEMS_ERR(SetType, T)                            \
    inline static _ListItem(SetType) _setItemsErrFunc(SetType)(SetType s)  \
    {                                                                      \
        QUIT(ERR_SET, __FILE__, __LINE__);                                 \
        _ListItem(SetType) items = _newListItemFunc(SetType)(s->scope, 0); \
        return items;                                                      \
    }

#define _DECLARE_SET_TYPE_PRINT(SetType, T)                                             \
    inline static void _printSetFunc(SetType)(SetType s, void (*printItemFunc)(T item)) \
    {                                                                                   \
        _ListItem(SetType) items = _setItemsFunc(SetType)(s);                           \
        printf("{");                                                                    \
        for (int i = 0; i < items->_size; i++)                                          \
        {                                                                               \
            printItemFunc(listAtQ(items, i));                                           \
            if (i < items->_size - 1)                                                   \
                printf(" , ");                                                          \
        }                                                                               \
        printf("}\n");                                                                  \
    }

#define _DECLARE_SET_TYPE_NEW(SetType, T, hashFunc, equalFunc)                                     \
    inline static SetType _newSetFunc(SetType)(Scope scope)                                        \
    {                                                                                              \
        SetType s = zeroAlloc(scope, sizeof(struct _SetTypeData(SetType)));                        \
        s->_table = (_ListListItem(SetType))_newListListItemFunc(SetType)(scope, SET_BUCKET_SIZE); \
        s->_size = 0;                                                                              \
        s->_nActiveBucket = 0;                                                                     \
        s->_hashFunc = hashFunc;                                                                   \
        s->_equalFunc = equalFunc;                                                                 \
        s->_add = _setAddFunc(SetType);                                                            \
        s->_contains = _setContainsFunc(SetType);                                                  \
        s->_remove = _setRemoveFunc(SetType);                                                      \
        s->_items = _setItemsFunc(SetType);                                                        \
        s->_itemsErr = _setItemsErrFunc(SetType);                                                  \
        s->_print = _printSetFunc(SetType);                                                        \
        s->scope = scope;                                                                          \
                                                                                                   \
        for (int i = 0; i < SET_BUCKET_SIZE; i++)                                                  \
        {                                                                                          \
            listAtQ(s->_table, i) = (_ListItem(SetType))_newListItemFunc(SetType)(scope, 0);       \
        }                                                                                          \
        return s;                                                                                  \
    }

#define DECLARE_SET_TYPE(SetType, T, hashFunc, equalFunc) \
    _DECLARE_SET_TYPE(SetType, T)                         \
    _DECLARE_SET_TYPE_ADD(SetType, T)                     \
    _DECLARE_SET_TYPE_CONTAINS(SetType, T)                \
    _DECLARE_SET_TYPE_REMOVE(SetType, T)                  \
    _DECLARE_SET_TYPE_ITEMS(SetType, T)                   \
    _DECLARE_SET_TYPE_ITEMS_ERR(SetType, T)               \
    _DECLARE_SET_TYPE_PRINT(SetType, T)                   \
    _DECLARE_SET_TYPE_NEW(SetType, T, hashFunc, equalFunc)

#define _isValidSet(s) ((s) != NULL && (s)->scope != NULL && (s)->scope->state == VALID)

#define _checkSetValid(s)                  \
    if (!_isValidSet(s))                   \
    {                                      \
        QUIT(ERR_SET, __FILE__, __LINE__); \
    }

#define setSize(s) (_isValidSet(s) ? (s)->_size : QUIT(ERR_SET, __FILE__, __LINE__).value)
#define setIsEmpty(s) (setSize(s) == 0)

#define setContains(s, item) (_isValidSet(s) ? ((s)->_contains)(s, item) : QUIT(ERR_SET, __FILE__, __LINE__).value)
#define setItems(s) (_isValidSet(s) ? ((s)->_items)(s) : (s)->_itemsErr(s))

#define setAdd(s, item)       \
    {                         \
        _checkSetValid(s);    \
        ((s)->_add)(s, item); \
    }

#define setRemove(s, item)       \
    {                            \
        _checkSetValid(s);       \
        ((s)->_remove)(s, item); \
    }

#define printSet(s, printItemFunc)           \
    {                                        \
        if (_isValidSet(s))                  \
        {                                    \
            ((s)->_print)(s, printItemFunc); \
        }                                    \
    }

static inline int hashInt(int x) { return x; }
static inline bool intEqual(int x1, int x2) { return x1 == x2; }

static inline int hashLong(long x) { return x; }
static inline bool longEqual(long x1, long x2) { return x1 == x2; }

DECLARE_SET_TYPE(SetInt, int, hashInt, intEqual);
DECLARE_SET_TYPE(SetLong, long, hashLong, longEqual);

#endif