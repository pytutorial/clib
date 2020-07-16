#include "list.h"
#include <stdio.h>

#define _quicksort(number,N, key_func, func){                                   \
   int i, j, pivot;                                                             \
   typeof(number[0]) temp;                                                      \
   if(N-1>0){                                                                   \
      pivot=0;                                                                  \
      i=0;                                                                      \
      j=N-1;                                                                    \
      while(i<j){                                                               \
         if(key_func != NULL)                                                   \
         {                                                                      \
            while(key_func(number[i])<=key_func(number[pivot])&&i<N-1)          \
               i++;                                                             \
            while(key_func(number[j])>key_func(number[pivot]))                  \
               j--;                                                             \
         }                                                                      \
         else                                                                   \
         {                                                                      \
             while(number[i]<=number[pivot]&&i<N-1)                            \
               i++;                                                             \
            while(number[j]>number[pivot])                                      \
               j--;                                                             \
         }                                                                      \
         if(i<j){                                                               \
            temp=number[i];                                                     \
            number[i]=number[j];                                                \
            number[j]=temp;                                                     \
         }                                                                      \
      }                                                                         \
      temp=number[pivot];                                                       \
      number[pivot]=number[j];                                                  \
      number[j]=temp;                                                           \
      func(number,j);                                                           \
      func(number+j+1,N-j-1);                                                   \
   }                                                                            \
}

int neg(int x) { return -x;}

void quicksort(int* number, int N) 
{
    int (*key_func)(int)  = NULL;
    _quicksort(number, N, neg, quicksort);
}

int main()
{
    Scope scope = newScope();
    ListInt lst = newList(scope);
    listPush(lst, 1);
    listPush(lst, 3);
    listPush(lst, 8);
    listPush(lst, 6);
    quicksort(lst->items, listSize(lst));
    printListFmt(lst, "%d");
    /*printf("Argmax: %d\n", listArgMax(lst));
    printf("Max: %d\n", listMax(lst));
    printf("Argmin: %d\n", listArgMin(lst));
    printf("Min: %d\n", listMin(lst));*/
    freeScope(scope);
    return 0;
}