#ifndef DA_H
#define DA_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#define da_struct(type, name) typedef struct {	\
	type* items;                                \
	uint32_t len;                               \
	uint32_t cap;                               \
} name;

#define da_init(da) do {                            \
	(da)->len = 0;                                  \
	(da)->cap = 2;                                  \
	(da)->items = malloc(2*sizeof((da)->items[0])); \
} while (0)

#define da_free(da) do { \
	free((da)->items);   \
} while (0)

#define da_append(da, elem) do {                                    \
	if ((da)->len >= (da)->cap) {                                   \
		(da)->cap *= 2;                                             \
		(da)->items = realloc((da)->items, (da)->cap*sizeof(elem)); \
	}                                                               \
	(da)->items[(da)->len++] = elem;                                \
} while (0)

#define da_insert(da, elem, idx) do {                               \
	if (idx >= (da)->len) {                                         \
		da_append((da), elem);                                      \
		continue;                                                   \
	}                                                               \
	if ((da)->len + 1 == (da)->cap) {                               \
		(da)->cap *= 2;                                             \
		(da)->items = realloc((da)->items, (da)->cap*sizeof(elem)); \
	}                                                               \
	memmove(&(da)->items[idx + 1], &(da)->items[idx],               \
		((da)->len - idx)*sizeof((da)->items[0]));                  \
	(da)->items[idx] = elem;                                        \
	(da)->len++;                                                    \
} while (0)

#endif // DA_H