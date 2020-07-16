#ifndef __LIST_H__
#define __LIST_H__
#include <stdio.h>
#include <stdlib.h>
#include "scope.h"

#define _ListData(T) struct {T* items; int _size; int _capacity; Scope scope;}

#define List(T) _ListData(T)*

#define newList(scope) ({                                                 \
    int size = sizeof(_ListData(void));                                   \
    (_ListData(void)*) data = zeroAlloc(scope, size);                     \
    data->scope = scope;                                                  \
    (void*) data;                                                         \
})

#define listSize(lst) (lst)->_size

#define _ensureListCap(lst, cap)                                          \
        {                                                                 \
            int newCap = 0;                                               \
            (lst)->items =                                                \
                ensureCapacity((lst)->scope,                              \
                               (lst)->items,                              \
                               sizeof(lst->items[0]),                     \
                               (lst)->_size,                              \
                               (lst)->_capacity,                          \
                               cap,                                       \
                               &newCap);                                  \
                                                                          \
            lst->_capacity = newCap;                                      \
        }

#define listPush(lst, item)                                               \
        {                                                                 \
            _ensureListCap(lst, 1 + (lst)->_size);                        \
            (lst)->items[(lst)->_size] = item;                            \
            (lst)->_size += 1;                                            \
        }

#define listPop(lst)    ({ if((lst)->_size <= 0) halt();  (lst)->items[--(lst)->_size];})

#define listGet(lst, i) ({ if((unsigned) (i) >= (lst)->_size) halt(); (lst)->items[i]; })
    
#define listRemoveAt(lst, index)                                          \
        {                                                                 \
            for (int i = index; i < (lst)->_size - 1; i++)                \
            {                                                             \
                (lst)->items[i] = (lst)->items[i + 1];                    \
            }                                                             \
            (lst)->_size -= 1;                                            \
        }

#define listClear(lst)          {(lst)->_size = 0;}

#define printList(lst, fmt)                                             \
    {                                                                   \
        printf("[");                                                    \
                                                                        \
        for (int i = 0; i < (lst)->_size; i++)                          \
        {                                                               \
            printf(fmt, (lst)->items[i]);                               \
            if (i < (lst)->_size - 1)                                   \
                printf(", ");                                           \
        }                                                               \
                                                                        \
        printf("]\n");                                                  \
    }

#define listMax(lst) \
    ({\
        if((lst)->_size == 0) halt(); \
        typeof((lst)->item[0]) max = (lst)->item[0]; \
        for(int i = 0; i < (lst)->_size;i++) \
        { \
            if((lst)->item[i] > max) max = (lst)->item[i]; \
        } \
        max; \
    })
    
typedef List(int) ListInt;
typedef List(long) ListLong;
typedef List(double) ListDouble;

#endif