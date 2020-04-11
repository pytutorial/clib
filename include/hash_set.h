#ifndef __HASH_SET_H__
#define __HASH_SET_H__

#include "list.h"
#include "object.h"

typedef struct {
    List(ListLong) _table;
    int _size;
    Scope scope;
}_HashSetLongData, *HashSetLong;

HashSetLong newHashSetLong(Scope scope, int bucketSize) ;
int hashSetLongSize(HashSetLong s);
bool hashSetLongContains(HashSetLong s, long value);
void hashSetLongAdd(HashSetLong s, long value);

#endif