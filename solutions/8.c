#include "../common.h"
#include <math.h>

typedef struct {
	int64_t x, y, z;
} box_t;

da_struct(box_t, input_t)

int gather_input(const char* filename, input_t* out) {
	FILE* f_in = fopen(filename, "r");
	if (f_in == NULL) { return 1; }

	da_init(out);
	box_t box; uint32_t num;
	while (fscanf(f_in, "%d", &num) == 1) {
		box.x = num;
		fgetc(f_in); // comma 1
		fscanf(f_in, "%d", &num);
		box.y = num;
		fgetc(f_in); // comma 2
		fscanf(f_in, "%d", &num);
		box.z = num;
		da_append(out, box);
	}
	fclose(f_in);
	return 0;
}

int64_t sld(box_t a, box_t b) {
	int64_t dx = a.x - b.x;
	int64_t dy = a.y - b.y;
	int64_t dz = a.z - b.z;
	return dx*dx + dy*dy + dz*dz;
}

typedef struct {
	int64_t d;
	uint32_t a, b;
	bool marked;
} dist_t;

int cmp_dists(const void* a, const void* b) {
	dist_t* da = (dist_t*)a;
	dist_t* db = (dist_t*)b;
	if (da->d < db->d) return -1;
	else if (da->d == db->d) return 0;
	return 1;
}

void part1(void* inp) {
	input_t* input = (input_t*)inp;
	uint64_t res = 0;

	dist_t* dists = calloc(input->len*input->len, sizeof(dist_t));
	uint32_t idx = 0;
	for (uint32_t i = 0; i < input->len; i++) {
		for (uint32_t j = i + 1; j < input->len; j++) {
			dists[idx++] = (dist_t) {
				.d = sld(input->items[i], input->items[j]),
				.a = i, .b = j, .marked = false
			};
		}
	}
	qsort(dists, idx, sizeof(dist_t), cmp_dists);

	uint64_t c1 = 0, c2 = 0, c3 = 0, c = 0;

	// 10 for example, 1000 for input
	uint32_t imax = 1000;
	for (uint32_t i = 0; i < imax; i++) {
		if (dists[i].marked) continue;
		hashset_t* circ = hset_init(sizeof(uint32_t), 2048);
		hset_add(circ, &dists[i].a); hset_add(circ, &dists[i].b);
		dists[i].marked = true;
		bool did = false;
		do {
			did = false;
			hashset_iter_t* circ_iter = hset_iter_create(circ);
			hashset_iter_t* circ_iter_next = circ_iter;
			uint32_t val = 0;
			while (circ_iter_next != NULL) {
				memcpy(&val, circ_iter_next->value, sizeof(uint32_t));
				for (uint32_t j = 0; j < imax; j++) {
					if (dists[j].marked) continue;
					if (dists[j].a == val) {
						hset_add(circ, &dists[j].b);
						dists[j].marked = true; did = true;
					}
					else if (dists[j].b == val) {
						hset_add(circ, &dists[j].a);
						dists[j].marked = true; did = true;
					}
				}
				circ_iter_next = circ_iter_next->next;
			}
			hset_iter_free(circ_iter);
		} while (did);
		c = circ->count;
		if (c > c1) {
			c3 = c2; c2 = c1; c1 = c;
		} else if (c < c1 && c > c2) {
			c3 = c2; c2 = c;
		} else if (c < c2 && c > c3) {
			c3 = c;
		}
		hset_free(circ);
	}

	res = c1 * c2 * c3;
	printf("part 1 answer: %lu\n", res);
	free(dists);
	return;
}

da_struct(hashset_t*, circuits_t)

#define NOT_FOUND 0xFFFFFFFF
uint32_t find_box_in_circuits(circuits_t* circs, uint32_t idx) {
	for (uint32_t i = 0; i < circs->len; i++) {
		hashset_iter_t* circ_iter = hset_iter_create(circs->items[i]);
		hashset_iter_t* copy = circ_iter;
		while (copy != NULL) {
			if (*(uint32_t*)copy->value == idx) {
				hset_iter_free(circ_iter); return i;
			}
			copy = copy->next;
		}
		hset_iter_free(circ_iter);
	}
	return NOT_FOUND;
}

void part2(void* inp) {
	input_t* input = (input_t*)inp;
	uint64_t res = 0;

	dist_t* dists = calloc(input->len*input->len, sizeof(dist_t));
	uint32_t idx = 0;
	for (uint32_t i = 0; i < input->len; i++) {
		for (uint32_t j = i + 1; j < input->len; j++) {
			dists[idx++] = (dist_t) {
				.d = sld(input->items[i], input->items[j]),
				.a = i, .b = j, .marked = false
			};
		}
	}
	qsort(dists, idx, sizeof(dist_t), cmp_dists);

	circuits_t circs = {0}; da_init(&(circs));

	for (uint32_t i = 0; i < idx; i++) {
		uint32_t a_idx = find_box_in_circuits(&circs, dists[i].a);
		uint32_t b_idx = find_box_in_circuits(&circs, dists[i].b);
		if (a_idx == NOT_FOUND && b_idx == NOT_FOUND) {
			hashset_t* new_circ = hset_init(sizeof(uint32_t), input->len);
			hset_add(new_circ, &dists[i].a);
			hset_add(new_circ, &dists[i].b);
			da_append(&(circs), new_circ);
		} else if (a_idx != NOT_FOUND && b_idx == NOT_FOUND) {
			hset_add(circs.items[a_idx], &dists[i].b);
		} else if (a_idx == NOT_FOUND && b_idx != NOT_FOUND) {
			hset_add(circs.items[b_idx], &dists[i].a);
		} else if (a_idx != b_idx) {
			hset_union(circs.items[a_idx], circs.items[b_idx]);
			hset_free(circs.items[b_idx]);
			da_remove(&(circs), b_idx);
		}

		if (circs.len == 1 && circs.items[0]->count == input->len) {
			res = input->items[dists[i].a].x *
				input->items[dists[i].b].x;
			break;
		}
	}

	printf("part 2 answer: %lu\n", res);
	free(dists);
	hset_free(circs.items[0]);
	da_free(&(circs));
	return;
}

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("provide an input file name\n");
		return 1;
	}

	input_t input = {0};
	if (gather_input(argv[1], &input)) {
		printf("something has gone horribly wrong!\n");
		return 1;
	}

	double time;

	time = time_function(part1, &input);
	printf("took %.10lf seconds to run part 1\n", time);

	time = time_function(part2, &input);
	printf("took %.10lf seconds to run part 2\n", time);

	da_free(&(input));

	return 0;
}