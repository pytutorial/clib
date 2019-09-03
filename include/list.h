#ifndef __LIST_H__
#define __LIST_H__
#include <stdio.h>
#include <stdlib.h>
#include "object.h"

#define ALWAYS_BOUND_CHECK 0

#define ERR_INVALID_LIST                        "Access an invalid list"
#define ERR_LIST_AT                             "Get element of an invalid list or index out of range"
#define ERR_POP_LIST                            "Pop an empty or invalid list"
#define ERR_LIST_INDEX_OUT_OF_RANGE             "List index out of range"
#define ERR_MODIFY_READONLY_LIST                "Modify a readonly list"
#define ERR_INVALID_listSlice                   "Get slice of invalid list or index out of range"
#define ERR_SOURCE_INDEX                        "Source index out of range"
#define ERR_DEST_INDEX                          "Destination index out of range"
#define ERR_SRC_DST_TYPE                        "Source and destination list having different types"

#define _ListTypeData(ListType)                 _##ListType##Data
#define _ensureListCapFunc(ListType)            _ensure##ListType##Cap
#define _listAddFunc(ListType)                  _##ListType##Add
#define _listResizeFunc(ListType)               _##ListType##Resize
#define _listPopFunc(ListType)                  _##ListType##Pop
#define _listRemoveAtFunc(ListType)             _##ListType##RemoveAt
#define _listClearFunc(ListType)                _##ListType##Clear
#define _listSliceFunc(ListType)                _##ListType##Slice
#define _printListFunc(ListType)                _print##ListType
#define _newListFunc(ListType)                  new##ListType

#define _DECLARE_LIST_TYPE(ListType, T)                                                                       \
    typedef struct _ListTypeData(ListType)                                                                    \
    {                                                                                                         \
        T *_items;                                                                                            \
        int _size;                                                                                            \
        int _capacity;                                                                                        \
        int _readonly;                                                                                        \
        void (*_add)(struct _ListTypeData(ListType) * lst, T item);                                           \
        T(*_pop) (struct _ListTypeData(ListType) * lst);                                                      \
        void (*_resize)(struct _ListTypeData(ListType) * lst, int size);                                      \
        void (*_removeAt)(struct _ListTypeData(ListType) * lst, int index);                                   \
        void (*_clear)(struct _ListTypeData(ListType) * lst);                                                 \
        struct _ListTypeData(ListType) * (*_slice)(struct _ListTypeData(ListType) * lst, int start, int end); \
        void (*_print)(struct _ListTypeData(ListType) * lst, void (*printItemFunc)(T item));                  \
        Scope scope;                                                                                          \
    }                                                                                                         \
    *ListType;

#define _DECLARE_LIST_TYPE_ENSURE_CAP(ListType, T)                         \
    inline static void _ensureListCapFunc(ListType)(ListType lst, int cap) \
    {                                                                      \
        if (lst->_capacity < cap)                                          \
        {                                                                  \
            int newCap = 0;                                                \
            lst->_items =                                                  \
                ensureCapacity(lst->scope,                                 \
                               lst->_items,                                \
                               sizeof(T),                                  \
                               lst->_size,                                 \
                               lst->_capacity,                             \
                               cap,                                        \
                               &newCap);                                   \
                                                                           \
            lst->_capacity = newCap;                                       \
        }                                                                  \
    }

#define _DECLARE_LIST_TYPE_ADD(ListType, T)                         \
    inline static void _listAddFunc(ListType)(ListType lst, T item) \
    {                                                               \
        _ensureListCapFunc(ListType)(lst, 1 + lst->_size);          \
        lst->_items[lst->_size] = item;                             \
        lst->_size += 1;                                            \
    }

#define _DECLARE_LIST_TYPE_RESIZE(ListType, T)                           \
    inline static void _listResizeFunc(ListType)(ListType lst, int size) \
    {                                                                    \
        _ensureListCapFunc(ListType)(lst, size);                         \
        lst->_size = size;                                               \
    }

#define _DECLARE_LIST_TYPE_POP(ListType, T)              \
    inline static T _listPopFunc(ListType)(ListType lst) \
    {                                                    \
        return lst->_items[--lst->_size];                \
    }

#define _DECLARE_LIST_TYPE_REMOVE_AT(ListType, T)                           \
    inline static void _listRemoveAtFunc(ListType)(ListType lst, int index) \
    {                                                                       \
        for (int i = index; i < lst->_size - 1; i++)                        \
        {                                                                   \
            lst->_items[i] = lst->_items[i + 1];                            \
        }                                                                   \
        lst->_size -= 1;                                                    \
    }

#define _DECLARE_LIST_TYPE_CLEAR(ListType, T)                 \
    inline static void _listClearFunc(ListType)(ListType lst) \
    {                                                         \
        _listResizeFunc(ListType)(lst, 0);                    \
    }

#define _DECLARE_LIST_TYPE_SLICE(ListType, T)                                           \
    inline static ListType _listSliceFunc(ListType)(ListType lst, int start, int end)   \
    {                                                                                   \
        ListType slice = zeroAlloc(lst->scope, sizeof(struct _ListTypeData(ListType))); \
        slice->_items = lst->_items + start;                                            \
        slice->_size = slice->_capacity = end - start;                                  \
        slice->_readonly = TRUE;                                                        \
        slice->_add = NULL;                                                             \
        slice->_pop = NULL;                                                             \
        slice->_resize = NULL;                                                          \
        slice->_removeAt = NULL;                                                        \
        slice->_clear = NULL;                                                           \
        slice->_slice = lst->_slice;                                                    \
        slice->_print = lst->_print;                                                    \
        slice->scope = lst->scope;                                                      \
        return slice;                                                                   \
    }

#define _DECLARE_LIST_TYPE_PRINT(ListType, T)                                                \
    inline static void _printListFunc(ListType)(ListType lst, void (*printItemFunc)(T item)) \
    {                                                                                        \
        printf("[");                                                                         \
                                                                                             \
        for (int i = 0; i < lst->_size; i++)                                                 \
        {                                                                                    \
            printItemFunc(lst->_items[i]);                                                   \
            if (i < lst->_size - 1)                                                          \
                printf(", ");                                                                \
        }                                                                                    \
                                                                                             \
        printf("]\n");                                                                       \
    }

#define _DECLARE_NEW_LIST(ListType, T)                                           \
    inline static ListType _newListFunc(ListType)(Scope scope, int size)         \
    {                                                                            \
        ListType lst = zeroAlloc(scope, sizeof(struct _ListTypeData(ListType))); \
        lst->_items = zeroAlloc(scope, size * sizeof(T));                        \
        lst->_size = lst->_capacity = size;                                      \
        lst->_readonly = FALSE;                                                  \
        lst->_add = _listAddFunc(ListType);                                      \
        lst->_pop = _listPopFunc(ListType);                                      \
        lst->_resize = _listResizeFunc(ListType);                                \
        lst->_removeAt = _listRemoveAtFunc(ListType);                            \
        lst->_clear = _listClearFunc(ListType);                                  \
        lst->_slice = _listSliceFunc(ListType);                                  \
        lst->_print = _printListFunc(ListType);                                  \
        lst->scope = scope;                                                      \
        return lst;                                                              \
    }

#define DECLARE_LIST_TYPE(ListType, T)                        \
    _DECLARE_LIST_TYPE(ListType, T)                           \
    _DECLARE_LIST_TYPE_ENSURE_CAP(ListType, T)                \
    _DECLARE_LIST_TYPE_ADD(ListType, T)                       \
    _DECLARE_LIST_TYPE_RESIZE(ListType, T)                    \
    _DECLARE_LIST_TYPE_POP(ListType, T)                       \
    _DECLARE_LIST_TYPE_REMOVE_AT(ListType, T)                 \
    _DECLARE_LIST_TYPE_CLEAR(ListType, T)                     \
    _DECLARE_LIST_TYPE_SLICE(ListType, T)                     \
    _DECLARE_LIST_TYPE_PRINT(ListType, T)                     \
    _DECLARE_NEW_LIST(ListType, T)

#define listAt(lst, i) (*(((unsigned)(i) < (unsigned)(lst)->_size && (lst)->scope->state == VALID) ? ((lst)->_items + (i)) : (typeof((lst)->_items))QUIT(ERR_LIST_AT, __FILE__, __LINE__).ptr))

#if ALWAYS_BOUND_CHECK
#define listAtQ(lst, i) listAt(lst, i)
#else
#define listAtQ(lst, i) ((lst)->_items[i])
#endif

#define _isValidList(lst) ((lst) != NULL && (lst)->scope != NULL && (lst)->scope->state == VALID)
#define _isModifiableList(lst) ((lst) != NULL && !(lst)->_readonly)

#define _checkListValid(lst)                        \
    if (!_isValidList(lst))                         \
    {                                               \
        QUIT(ERR_INVALID_LIST, __FILE__, __LINE__); \
    }

#define _checkListModifiable(lst)                           \
    if (!_isModifiableList(lst))                            \
    {                                                       \
        QUIT(ERR_MODIFY_READONLY_LIST, __FILE__, __LINE__); \
    }

#define listAdd(lst, item)         \
    {                              \
        _checkListValid(lst);      \
        _checkListModifiable(lst); \
        (lst)->_add(lst, item);    \
    }

#define listPop(lst) ( \
    (_isValidList(lst) && _isModifiableList(lst)) ? (lst)->_pop(lst) : *((typeof((lst)->_items))QUIT(ERR_POP_LIST, __FILE__, __LINE__).ptr))

#define listRemoveAt(lst, index)                                   \
    {                                                              \
        _checkListValid(lst);                                      \
        _checkListModifiable(lst);                                 \
                                                                   \
        if ((unsigned)(index) >= (lst)->_size)                     \
        {                                                          \
            QUIT(ERR_LIST_INDEX_OUT_OF_RANGE, __FILE__, __LINE__); \
        }                                                          \
                                                                   \
        (lst)->_removeAt(lst, index);                              \
    }

#define listResize(lst, size)      \
    {                              \
        _checkListValid(lst);      \
        _checkListModifiable(lst); \
        (lst)->_resize(lst, size); \
    }

#define clearList(lst)             \
    {                              \
        _checkListValid(lst);      \
        _checkListModifiable(lst); \
        (lst)->_clear(lst);        \
    }

#define listSize(lst) (_isValidList(lst) ? (lst)->_size : QUIT(ERR_INVALID_LIST, __FILE__, __LINE__).value)

#define listIsEmpty(lst) (_isValidList(lst) ? (lst)->_size == 0 : QUIT(ERR_INVALID_LIST, __FILE__, __LINE__).value)

#define listDataPtr(lst) (_isValidList(lst) ? (lst)->_items : (typeof(lst->_items))QUIT(ERR_INVALID_LIST, __FILE__, __LINE__).ptr)

#define _checkRange(lst, start, end) (start >= 0 && start <= end && end <= listSize(lst))

#define listSlice(lst, start, end) ((_isValidList(lst) && _checkRange(lst, start, end)) ? (lst)->_slice(lst, start, end) : (typeof(lst))QUIT(ERR_INVALID_listSlice, __FILE__, __LINE__).ptr)

#define listCopy(dst, dstStart, src, srcStart, srcEnd)                  \
    {                                                                   \
        int srcSize = listSize(src);                                    \
        int dstSize = listSize(dst);                                    \
        if (srcStart < 0 || srcEnd < srcStart || srcEnd > srcSize)      \
        {                                                               \
            QUIT(ERR_SOURCE_INDEX, __FILE__, __LINE__);                 \
        }                                                               \
                                                                        \
        int len = srcEnd - srcStart;                                    \
        if (dstStart < 0 || dstStart + len > dstSize)                   \
        {                                                               \
            QUIT(ERR_DEST_INDEX, __FILE__, __LINE__);                   \
        }                                                               \
                                                                        \
        if (sizeof(src->_items[0]) != sizeof(dst->_items[0]))           \
        {                                                               \
            QUIT(ERR_SRC_DST_TYPE, __FILE__, __LINE__);                 \
        }                                                               \
                                                                        \
        memcpy((dst)->_items + dstStart,                                \
               (src)->_items + srcStart, len * sizeof(src->_items[0])); \
    }

#define printList(lst, printItemFunc)      \
    if (_isValidList(lst))                 \
    {                                      \
        (lst)->_print(lst, printItemFunc); \
    }

DECLARE_LIST_TYPE(ListInt, int);
DECLARE_LIST_TYPE(ListChar, char);
DECLARE_LIST_TYPE(ListLong, long);
DECLARE_LIST_TYPE(ListShort, short);
DECLARE_LIST_TYPE(ListDouble, double);
DECLARE_LIST_TYPE(ListFloat, float);

#endif