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
	uint64_t count;
} hashset_t;

hashset_t* hset_init(uint64_t sz, uint64_t cap);
void hset_add(hashset_t* hset, void* elem);
bool hset_remove(hashset_t* hset, void* elem);
bool hset_contains(hashset_t* hset, void* elem);
void hset_free(hashset_t* hset);

typedef struct hashset_iter_t {
	struct hashset_iter_t* next;
	void* value;
	uint64_t sz;
} hashset_iter_t;

hashset_iter_t* hset_iter_create(hashset_t* hset);
void hset_iter_free(hashset_iter_t* hset_iter);

#endif // HASHSET_H