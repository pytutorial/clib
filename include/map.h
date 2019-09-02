#ifndef __MAP__H_
#define __MAP__H_
#include "list.h"
#include "object.h"

#define ERR_MAP                                 "Access an invalid map"
#define MAP_BUCKET_SIZE                         1024

#define _ItemType(K2V)                          Item##K2V
#define _MapTypeData(K2V)                       _Map##K2V##Data
#define _MapType(K2V)                           Map##K2V
#define _ListItemType(K2V)                      ListItem##K2V
#define _ListListItemType(K2V)                  ListListItem##K2V
#define _newListItemFunc(K2V)                   newListItem##K2V
#define _newListListItemFunc(K2V)               newListListItem##K2V
#define _mapGetFunc(K2V)                        _map##K2V##Get
#define _mapGetErrFunc(K2V)                     _map##K2V##GetErr
#define _mapPutFunc(K2V)                        _map##K2V##Put
#define _mapContainsKeyFunc(K2V)                _map##K2V##Contains
#define _mapRemoveFunc(K2V)                     _map##K2V##Remove
#define _mapItemsFunc(K2V)                      _map##K2V##Items
#define _mapItemsErrFunc(K2V)                   _map##K2V##ItemsErr
#define _printMapFunc(K2V)                      _printMap##K2V
#define _newMapFunc(K2V)                        newMap##K2V

#define _DECLARE_MAP_TYPE(K2V, K, V)                                                                                \
    typedef struct                                                                                                  \
    {                                                                                                               \
        K key;                                                                                                      \
        V value;                                                                                                    \
    } _ItemType(K2V);                                                                                               \
    DECLARE_LIST(_ListItemType(K2V), _ItemType(K2V));                                                               \
    DECLARE_LIST(_ListListItemType(K2V), _ListItemType(K2V));                                                       \
    typedef struct _MapTypeData(K2V)                                                                                \
    {                                                                                                               \
        _ListListItemType(K2V) _table;                                                                              \
        int _size;                                                                                                  \
        int _nActiveBucket;                                                                                         \
        int (*_hashFunc)(K);                                                                                        \
        bool (*_equalFunc)(K, K);                                                                                   \
        V (*_get) (struct _MapTypeData(K2V) * m, K key, V defaultValue);                                            \
        V (*_getErr) (V defaultValue);                                                                              \
        void (*_put)(struct _MapTypeData(K2V) * m, K key, V value);                                                 \
        bool (*_containsKey)(struct _MapTypeData(K2V) * m, K key);                                                  \
        void (*_remove)(struct _MapTypeData(K2V) * m, K key);                                                       \
        _ListItemType(K2V) (*_items)(struct _MapTypeData(K2V) * m);                                                 \
        _ListItemType(K2V) (*_itemsErr)(struct _MapTypeData(K2V) * m);                                              \
        void (*_print)(struct _MapTypeData(K2V) * m, void (*printKeyFunc)(K key), void (*printValueFunc)(V value)); \
        Scope scope;                                                                                                \
    }                                                                                                               \
    *_MapType(K2V);

#define _DECLARE_MAP_TYPE_GET(K2V, K, V)                                     \
    inline static V _mapGetFunc(K2V)(_MapType(K2V) m, K key, V defaultValue) \
    {                                                                        \
        int hash = (m->_hashFunc)(key);                                      \
        int index = (unsigned)hash % m->_table->_size;                       \
        _ListItemType(K2V) lstItem = listAtQ(m->_table, index);              \
        for (int i = 0; i < lstItem->_size; i++)                             \
        {                                                                    \
            if ((m->_equalFunc)(key, listAtQ(lstItem, i).key))               \
            {                                                                \
                return listAtQ(lstItem, i).value;                            \
            }                                                                \
        }                                                                    \
        return defaultValue;                                                 \
    }

#define _DECLARE_MAP_TYPE_GET_ERR(K2V, K, V)            \
    inline static V _mapGetErrFunc(K2V)(V defaultValue) \
    {                                                   \
        QUIT(ERR_MAP, __FILE__, __LINE__);              \
        return defaultValue;                            \
    }

#define _DECLARE_MAP_TYPE_PUT(K2V, K, V)                                 \
    inline static void _mapPutFunc(K2V)(_MapType(K2V) m, K key, V value) \
    {                                                                    \
        int hash = (m->_hashFunc)(key);                                  \
        int index = (unsigned)hash % m->_table->_size;                   \
        _ListItemType(K2V) lstItem = listAtQ(m->_table, index);          \
                                                                         \
        if (lstItem->_size == 0)                                         \
        {                                                                \
            int nActiveBucket = ++(m->_nActiveBucket);                   \
                                                                         \
            if (nActiveBucket > m->_table->_size / 2)                    \
            {                                                            \
                listResize(m->_table, 3 * m->_table->_size / 2);         \
            }                                                            \
        }                                                                \
                                                                         \
        int i = 0;                                                       \
        for (i = 0; i < lstItem->_size; i++)                             \
        {                                                                \
            if ((m->_equalFunc)(key, listAtQ(lstItem, i).key))           \
                break;                                                   \
        }                                                                \
        if (i < lstItem->_size)                                          \
        {                                                                \
            listAtQ(lstItem, i).value = value;                           \
        }                                                                \
        else                                                             \
        {                                                                \
            listAdd(lstItem, ((_ItemType(K2V)){key, value}));            \
            m->_size += 1;                                               \
        }                                                                \
    }

#define _DECLARE_MAP_TYPE_CONTAINS(K2V, K, V)                           \
    inline static bool _mapContainsKeyFunc(K2V)(_MapType(K2V) m, K key) \
    {                                                                   \
        int hash = (m->_hashFunc)(key);                                 \
        int index = (unsigned)hash % m->_table->_size;                  \
        _ListItemType(K2V) lstItem = listAtQ(m->_table, index);         \
        for (int i = 0; i < lstItem->_size; i++)                        \
        {                                                               \
            if ((m->_equalFunc)(key, listAtQ(lstItem, i).key))          \
            {                                                           \
                return TRUE;                                            \
            }                                                           \
        }                                                               \
        return FALSE;                                                   \
    }

#define _DECLARE_MAP_TYPE_REMOVE(K2V, K, V)                        \
    inline static void _mapRemoveFunc(K2V)(_MapType(K2V) m, K key) \
    {                                                              \
        int hash = (m->_hashFunc)(key);                            \
        int index = (unsigned)hash % m->_table->_size;             \
        _ListItemType(K2V) lstItem = listAtQ(m->_table, index);    \
        for (int i = 0; i < lstItem->_size; i++)                   \
        {                                                          \
            if ((m->_equalFunc)(key, listAtQ(lstItem, i).key))     \
            {                                                      \
                listRemoveAt(lstItem, i);                          \
                return;                                            \
            }                                                      \
        }                                                          \
    }

#define _DECLARE_MAP_TYPE_ITEMS(K2V, K, V)                               \
    inline static _ListItemType(K2V) _mapItemsFunc(K2V)(_MapType(K2V) m) \
    {                                                                    \
        _ListItemType(K2V) items = _newListItemFunc(K2V)(m->scope, 0);   \
        for (int index = 0; index < m->_table->_size; index++)           \
        {                                                                \
            _ListItemType(K2V) lstItem = listAtQ(m->_table, index);      \
            for (int i = 0; i < lstItem->_size; i++)                     \
            {                                                            \
                listAdd(items, listAtQ(lstItem, i));                     \
            }                                                            \
        }                                                                \
        return items;                                                    \
    }

#define _DECLARE_MAP_TYPE_ITEMS_ERR(K2V, K, V)                              \
    inline static _ListItemType(K2V) _mapItemsErrFunc(K2V)(_MapType(K2V) m) \
    {                                                                       \
        QUIT(ERR_MAP, __FILE__, __LINE__);                                  \
        _ListItemType(K2V) items = _newListItemFunc(K2V)(m->scope, 0);      \
        return items;                                                       \
    }

#define _DECLARE_MAP_TYPE_PRINT(K2V, K, V)                                                                               \
    inline static void _printMapFunc(K2V)(_MapType(K2V) m, void (*printKeyFunc)(K key), void (*printValueFunc)(V value)) \
    {                                                                                                                    \
        _ListItemType(K2V) items = _mapItemsFunc(K2V)(m);                                                                \
        printf("{");                                                                                                     \
        for (int i = 0; i < items->_size; i++)                                                                           \
        {                                                                                                                \
            printKeyFunc(listAtQ(items, i).key);                                                                         \
            printf(" : ");                                                                                               \
            printValueFunc(listAtQ(items, i).value);                                                                     \
            if (i < items->_size - 1)                                                                                    \
                printf(" , ");                                                                                           \
        }                                                                                                                \
        printf("}\n");                                                                                                   \
    }

#define _DECLARE_MAP_TYPE_NEW(K2V, K, V, hashFunc, equalFunc)                                  \
    inline static _MapType(K2V) _newMapFunc(K2V)(Scope scope)                                  \
    {                                                                                          \
        _MapType(K2V) m = zeroAlloc(scope, sizeof(struct _MapTypeData(K2V)));                  \
        m->_table = (_ListListItemType(K2V))_newListListItemFunc(K2V)(scope, MAP_BUCKET_SIZE); \
        m->_size = 0;                                                                          \
        m->_nActiveBucket = 0;                                                                 \
        m->_hashFunc = hashFunc;                                                               \
        m->_equalFunc = equalFunc;                                                             \
        m->_get = _mapGetFunc(K2V);                                                            \
        m->_getErr = _mapGetErrFunc(K2V);                                                      \
        m->_put = _mapPutFunc(K2V);                                                            \
        m->_containsKey = _mapContainsKeyFunc(K2V);                                            \
        m->_remove = _mapRemoveFunc(K2V);                                                      \
        m->_items = _mapItemsFunc(K2V);                                                        \
        m->_itemsErr = _mapItemsErrFunc(K2V);                                                  \
        m->_print = _printMapFunc(K2V);                                                        \
        m->scope = scope;                                                                      \
                                                                                               \
        for (int i = 0; i < MAP_BUCKET_SIZE; i++)                                              \
        {                                                                                      \
            listAtQ(m->_table, i) = (_ListItemType(K2V))_newListItemFunc(K2V)(scope, 0);       \
        }                                                                                      \
        return m;                                                                              \
    }

#define DECLARE_MAP_TYPE(K2V, K, V, hashFunc, equalFunc) \
    _DECLARE_MAP_TYPE(K2V, K, V)                         \
    _DECLARE_MAP_TYPE_GET(K2V, K, V)                     \
    _DECLARE_MAP_TYPE_GET_ERR(K2V, K, V)                 \
    _DECLARE_MAP_TYPE_PUT(K2V, K, V)                     \
    _DECLARE_MAP_TYPE_CONTAINS(K2V, K, V)                \
    _DECLARE_MAP_TYPE_REMOVE(K2V, K, V)                  \
    _DECLARE_MAP_TYPE_ITEMS(K2V, K, V)                   \
    _DECLARE_MAP_TYPE_ITEMS_ERR(K2V, K, V)               \
    _DECLARE_MAP_TYPE_PRINT(K2V, K, V)                   \
    _DECLARE_MAP_TYPE_NEW(K2V, K, V, hashFunc, equalFunc)

#define _isValidMap(m) ((m) != NULL && (m)->scope != NULL && (m)->scope->state == VALID)

#define _checkMapValid(m)                  \
    if (!_isValidMap(m))                   \
    {                                      \
        QUIT(ERR_MAP, __FILE__, __LINE__); \
    }

#define mapSize(m) (_isValidMap(m) ? (m)->_size : QUIT(ERR_MAP, __FILE__, __LINE__).value)
#define mapIsEmpty(m) (mapSize(m) == 0)

#define mapGet(m, key, defaultValue) (_isValidMap(m) ? ((m)->_get)(m, key, defaultValue) : (m)->_getErr(defaultValue))
#define mapContainsKey(m, key) (_isValidMap(m) ? ((m)->_containsKey)(m, key) : QUIT(ERR_MAP, __FILE__, __LINE__).value)
#define mapItems(m) (_isValidMap(m) ? ((m)->_items)(m) : (m)->_itemsErr(m))

#define mapPut(m, key, value)       \
    {                               \
        _checkMapValid(m);          \
        ((m)->_put)(m, key, value); \
    }

#define mapRemove(m, key)       \
    {                           \
        _checkMapValid(m);      \
        ((m)->_remove)(m, key); \
    }

#define printMap(m, printKeyFunc, printValueFunc)           \
    {                                                       \
        if (_isValidMap(m))                                 \
        {                                                   \
            ((m)->_print)(m, printKeyFunc, printValueFunc); \
        }                                                   \
    }

static inline int hashInt(int x) { return x; }
static inline bool intEqual(int x1, int x2) { return x1 == x2; }

DECLARE_MAP_TYPE(Int2Int, int, int, hashInt, intEqual);

#endif