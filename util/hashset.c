#include "hashset.h"

hashset_t* hset_init(uint64_t sz, uint64_t cap) {
	hashset_t* hset = malloc(sz*cap);
	if (hset == NULL) return NULL;
	hset->count = 0;
	hset->items = calloc(cap, sizeof(void*));
	hset->sz = sz;
	hset->cap = cap;
	return hset;
}

static uint64_t djb2(void* elem, uint64_t sz) {
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
	hset->count++;
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
	hset->count--;
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

// a = a | b
// assumes a and b are hashsets of the same type
void hset_union(hashset_t* a, hashset_t* b) {
	hashset_iter_t* b_iter = hset_iter_create(b);
	hashset_iter_t* copy = b_iter;
	while (copy != NULL) {
		hset_add(a, copy->value);
		copy = copy->next;
	}
	hset_iter_free(b_iter);
	return;
}

void hset_free(hashset_t* hset) {
	for (uint64_t i = 0; i < hset->cap; i++)
		free(hset->items[i]);
	free(hset->items);
	free(hset);
	return;
}

static void hset_iter_append(hashset_iter_t* hset_iter, void* value) {
	hashset_iter_t* n = malloc(sizeof(hashset_iter_t));
	n->sz = hset_iter->sz;
	n->value = value;
	n->next = NULL;
	while (hset_iter->next != NULL) hset_iter = hset_iter->next;
	hset_iter->next = n;
	return;
}

hashset_iter_t* hset_iter_create(hashset_t* hset) {
	hashset_iter_t* hset_iter = malloc(sizeof(hashset_iter_t));
	hset_iter->sz = hset->sz;
	hset_iter->value = NULL;
	hset_iter->next = NULL;
	for (uint64_t i = 0; i < hset->cap; i++) {
		if (hset->items[i] != NULL) {
			if (hset_iter->value == NULL) {
				hset_iter->value = hset->items[i];
			}
			else hset_iter_append(hset_iter, hset->items[i]);
		}
	}
	return hset_iter;
}

void hset_iter_free(hashset_iter_t* hset_iter) {
	while (hset_iter != NULL) {
		hashset_iter_t* tmp = hset_iter;
		hset_iter = hset_iter->next;
		free(tmp);
	}
	return;
}