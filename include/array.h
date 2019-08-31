#ifndef __ARRAY_H__
#define __ARRAY_H__

#include <stdlib.h>
#include <string.h>

#define Array(type) \
    struct          \
    {               \
        type *data; \
        int size;   \
    }

#define EMPTY_ARRAY \
    {               \
        NULL, 0     \
    }

#define gen_var(a) a##__gen

#define declare_array(arr, arr_type, sz)           \
    arr_type arr = EMPTY_ARRAY;                    \
    typeof(arr.data[0]) gen_var(arr)[sz];          \
    arr.data = gen_var(arr);                       \
    memset(arr.data, 0, sz * sizeof(arr.data[0])); \
    arr.size = sz;

inline static void *_err_arr_index(char *arrname, int index, int length)
{
    printf("\nError : array index out of range : arr=%s , index=%d, length=%d\n", arrname, index, length);
    exit(-1);
    return NULL;
}

#define at(arr, i) ((arr).data[i])
#define at_s(arr, i) ( \
    *(((unsigned)(i) < (arr).size) ? ((arr).data + (i)) : (typeof((arr).data))_err_arr_index(#arr, i, (arr).size)))

typedef Array(int) ArrayInt;
typedef Array(char) ArrayChar;
typedef Array(long) ArrayLong;
typedef Array(short) ArrayShort;
typedef Array(double) ArrayDouble;
typedef Array(float) ArrayFloat;
typedef Array(unsigned char) ArrayByte;
typedef Array(void *) ArrayPtr;

#endif