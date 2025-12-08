#include "hashset.h"

hashset_t* hset_init(uint64_t sz, uint64_t cap) {
	hashset_t* hset = malloc(sz*cap);
	if (hset == NULL) return NULL;
	hset->items = calloc(cap, sizeof(void*));
	hset->sz = sz;
	hset->cap = cap;
	return hset;
}

uint64_t djb2(void* elem, uint64_t sz) {
	uint64_t hash = 5381;
	unsigned char* data = (unsigned char*)elem;
	for (uint64_t i = 0; i < sz; i++) {
		hash = ((hash << 5) + hash) + data[i];
	}
	return hash;
}

void hset_add(hashset_t* hset, void* elem) {
	uint64_t hash = djb2(elem, hset->sz) % hset->cap;
	uint64_t idx = hash;
	while (hset->items[idx] != NULL) {
		if (memcmp(elem, hset->items[idx], hset->sz) == 0)
			return;
		idx++;
		if (idx == hash) {
			fprintf(stderr, "went for a loop, set full!");
			return;
		}
		if (idx >= hset->cap) idx = 0;
	}
	hset->items[idx] = malloc(hset->sz);
	memcpy(hset->items[idx], elem, hset->sz);
	return;
}

bool hset_remove(hashset_t* hset, void* elem) {
	uint64_t hash = djb2(elem, hset->sz) % hset->cap;
	uint64_t idx = hash;
	if (hset->items[idx] == NULL) return false;
	while (hset->items[idx] != NULL &&
	memcmp(elem, hset->items[idx], hset->sz) != 0) {
		idx++;
		if (idx == hash) return false;
		if (idx >= hset->cap) idx = 0;
	}
	if (hset->items[idx] == NULL) return false;
	free(hset->items[idx]);
	hset->items[idx] = NULL;
	return true;
}

bool hset_contains(hashset_t* hset, void* elem) {
	uint64_t hash = djb2(elem, hset->sz) % hset->cap;
	uint64_t idx = hash;
	if (hset->items[idx] == NULL) return false;
	while (hset->items[idx] != NULL &&
	memcmp(elem, hset->items[idx], hset->sz) != 0) {
		idx++;
		if (idx == hash) return false;
		if (idx >= hset->cap) idx = 0;
	}
	if (hset->items[idx] == NULL) return false;
	return true;
}

void hset_free(hashset_t* hset) {
	for (uint64_t i = 0; i < hset->cap; i++)
		free(hset->items[i]);
	free(hset->items);
	free(hset);
}