#ifndef HASHSET_H
#define HASHSET_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

typedef struct {
	void** items;
	uint64_t sz; // element size
	uint64_t cap;
} hashset_t;

uint64_t djb2(void* elem, uint64_t sz);

hashset_t* hset_init(uint64_t sz, uint64_t cap);
void hset_add(hashset_t* hset, void* elem);
bool hset_remove(hashset_t* hset, void* elem);
bool hset_contains(hashset_t* hset, void* elem);
void hset_free(hashset_t* hset);

#endif // HASHSET_H