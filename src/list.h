#ifndef __LIST_H__
#define __LIST_H__
#include <stdio.h>
#include <stdlib.h>
#include "scope.h"

#define _ListData(T) struct {T* items; int _size; int _capacity; Scope scope;}

#define List(T) _ListData(T)*

#define newList(scope) ({                                                       \
    int size = sizeof(_ListData(void));                                         \
    List(void) lst = zeroAlloc(scope, size);                                    \
    lst->scope = scope;                                                         \
    (void*) lst;                                                                \
})

#define listSize(lst) (lst)->_size

#define _ensureListCap(lst, cap)                                                \
    {                                                                           \
        int newCap = 0;                                                         \
        (lst)->items =                                                          \
            ensureCapacity((lst)->scope,                                        \
                           (lst)->items,                                        \
                           sizeof(lst->items[0]),                               \
                           (lst)->_size,                                        \
                           (lst)->_capacity,                                    \
                           cap,                                                 \
                           &newCap);                                            \
                                                                                \
        lst->_capacity = newCap;                                                \
    }

#define listPush(lst, item)                                                     \
    {                                                                           \
        _ensureListCap(lst, 1 + (lst)->_size);                                  \
        (lst)->items[(lst)->_size] = item;                                      \
        (lst)->_size += 1;                                                      \
    }

#define listPop(lst)    ({ if((lst)->_size <= 0) halt();  (lst)->items[--(lst)->_size];})

#define listGet(lst, i) ({ if((unsigned) (i) >= (lst)->_size) halt(); (lst)->items[i]; })
    
#define listRemoveAt(lst, index)                                                \
    {                                                                           \
        for (int i = index; i < (lst)->_size - 1; i++)                          \
        {                                                                       \
            (lst)->items[i] = (lst)->items[i + 1];                              \
        }                                                                       \
        (lst)->_size -= 1;                                                      \
    }

#define listClear(lst)          {(lst)->_size = 0;}

#define listFilterIndexes(scope, lst, func)                                     \
    ({                                                                          \
        ListInt indexes = newList(scope);                                       \
        for(int i = 0; i < listSize(lst); i++)                                  \
        {                                                                       \
            if(func((lst)->items[i])) listPush(indexes, i);                     \
        }                                                                       \
        indexes;                                                                \
    })

#define listFilterItems(scope, lst, func)                                       \
    ({                                                                          \
        typeof(lst) items = newList(scope);                                     \
        for(int i = 0; i < listSize(lst); i++)                                  \
        {                                                                       \
            if(func((lst)->items[i])) listPush(items, lst->items[i]);           \
        }                                                                       \
        indexes;                                                                \
    })
    
#define listGroupBy(scope, lst, key_func)                                       \
    ({                                                                          \
        HashMapLong_ListInt groups = newHashMap(scope);                         \
        for(int i = 0; i < listSize(lst); i++)                                  \
        {                                                                       \
            unsigned long key = key_func(lst->items[i]);                        \
            ListInt indexes = hashMapGetOrDefault(groups, key, NULL);           \
            if(indexes == NULL)                                                 \
            {                                                                   \
                indexes = (ListInt) newList(scope);                             \
                hashMapPut(groups, indexes);                                    \
            }                                                                   \
            listPush(indexes, i);                                               \
        }                                                                       \
        groups;                                                                 \
    })
    
#define subListByIndexes(scope, lst, indexes)                                   \
    ({                                                                          \
        typeof(lst) subLst = newList(scope);                                    \
        for(int i = 0; i < listSize(indexes); i++)                              \
        {                                                                       \
            unsigned index = indexes.items[i];                                  \
            listPush(subLst, listGet(lst, index));                              \
        }                                                                       \
        subLst;                                                                 \
    })                                                                          \
    
#define subList(scope, lst, start, end)                                         \
    ({                                                                          \
        if(start < 0) start = 0;                                                \
        if(end > listSize(lst)) end = listSize(lst);                            \
        typeof(lst) subLst = newList(scope);                                    \
        for(int i = start; i < end; i++)                                        \
        {                                                                       \
            listPush(subLst, lst->items[i]);                                    \
        }                                                                       \
        subLst;                                                                 \
    })
    
#define printListFmt(lst, fmt)                                                  \
    {                                                                           \
        printf("[");                                                            \
                                                                                \
        for (int i = 0; i < (lst)->_size; i++)                                  \
        {                                                                       \
            printf(fmt, (lst)->items[i]);                                       \
            if (i < (lst)->_size - 1)                                           \
                printf(", ");                                                   \
        }                                                                       \
                                                                                \
        printf("]");                                                            \
    }

#define printList(lst, printItemFunc)                                           \
    {                                                                           \
        printf("[");                                                            \
                                                                                \
        for (int i = 0; i < (lst)->_size; i++)                                  \
        {                                                                       \
            printItemFunc((lst)->items[i]);                                     \
            if (i < (lst)->_size - 1)                                           \
                printf(", ");                                                   \
        }                                                                       \
                                                                                \
        printf("]");                                                            \
    }

    
#define listArgMax(lst)                                                         \
    ({                                                                          \
        if((lst)->_size == 0) halt();                                           \
        int imax = 0;                                                           \
        for(int i = 0; i < (lst)->_size; i++)                                   \
        {                                                                       \
            if((lst)->items[i] > (lst)->items[imax]) imax = i;                  \
        }                                                                       \
        imax;                                                                   \
    })
 
#define listMax(lst) ({ int imax = listArgMax(lst); (lst)->items[imax]; })
    
#define listArgMin(lst)                                                         \
    ({                                                                          \
        if((lst)->_size == 0) halt();                                           \
        int imin = 0;                                                           \
        for(int i = 0; i < (lst)->_size; i++)                                   \
        {                                                                       \
            if((lst)->items[i] < (lst)->items[imin]) imin = i;                  \
        }                                                                       \
        imin;                                                                   \
    })    
    
#define listMin(lst) ({ int imin = listArgMin(lst); (lst)->items[imin]; })
    
typedef List(int) ListInt;
typedef List(long) ListLong;
typedef List(double) ListDouble;

#endif