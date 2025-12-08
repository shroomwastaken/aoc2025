#include "../common.h"
#include <math.h>

typedef struct {
	int32_t x, y, z;
} box_t;

// pair of indexes
// marked is for getting circuits
typedef struct {
	uint64_t a, b;
	bool marked;
} conn_t;

da_struct(box_t, input_t)
da_struct(conn_t, conns_t)
da_struct(uint32_t, idxarr_t)

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

double sld(box_t a, box_t b) {
	return sqrt(pow((double)(a.x - b.x), 2) +
				pow((double)(a.y - b.y), 2) +
				pow((double)(a.z - b.z), 2));
}

void print_box(box_t b) {
	printf("[%d %d %d]", b.x, b.y, b.z);
}

bool are_connected(uint32_t a, uint32_t b, conns_t* conns) {
	for (uint64_t i = 0; i < conns->len; i++) {
		if (conns->items[i].a == a && conns->items[i].b == b) {
			return true;
		} else if (conns->items[i].a == b && conns->items[i].b == a) {
			return true;
		}
	}
	return false;
}

// sets a and b to indices of closest unconnected boxes
// returns true if found at least one pair
bool closest(input_t* input, uint32_t* a, uint32_t* b, conns_t* conns) {
	bool done = false;
	double mind = 10000000, dist = 0;
	for (uint32_t i = 0; i < input->len - 1; i++) {
		for (uint32_t j = i + 1; j < input->len; j++) {
			dist = sld(input->items[i], input->items[j]);
			bool conn = are_connected(i, j, conns);
			if (dist < mind && !conn) {
				done = true;
				*a = i;
				*b = j;
				mind = dist;
			}
		}
	}
	return done;
}

// slow as hell but i dont have a hashset
void add_unique(idxarr_t* arr, uint32_t elem) {
	for (uint32_t i = 0; i < arr->len; i++) {
		if (arr->items[i] == elem) return;
	}
	da_append(arr, elem);
}

void part1(void* inp) {
	input_t* input = (input_t*)inp;
	uint64_t res = 0;

	conns_t conns = {0};
	da_init(&(conns));

	uint32_t a, b; conn_t conn;
	bool good = true;
	uint32_t count = 0;
	while ((good = closest(input, &a, &b, &conns)) && count < 1000) {
		count++;
		conn = (conn_t){.a = a, .b = b, false};
		da_append(&(conns), conn);
	}

	idxarr_t* a1 = malloc(sizeof(idxarr_t)); da_init(a1);
	idxarr_t* a2 = malloc(sizeof(idxarr_t)); da_init(a2);
	idxarr_t* circ = malloc(sizeof(idxarr_t)); da_init(circ);
	idxarr_t* tmp;
	bool done = false, new_circ = true;
	uint32_t circ_size, c1 = 0, c2 = 0, c3 = 0;

	while (true) {
		if (new_circ) {
			done = true;
			for (uint32_t i = 0; i < conns.len; i++) {
				if (!conns.items[i].marked) {
					da_append(a1, conns.items[i].a);
					da_append(a1, conns.items[i].b);
					add_unique(circ, conns.items[i].a);
					add_unique(circ, conns.items[i].b);
					done = false;
					conns.items[i].marked = true;
					break;
				}
			}
			if (done) break;
			new_circ = false;
			circ_size = a1->len;
			printf("added: ");
			for (uint32_t i = 0; i < a1->len; i++) {
				printf("%u ", a1->items[i]);
			}
			putchar('\n');
		}
		for (uint32_t i = 0; i < a1->len; i++) {
			for (uint32_t j = 0; j < conns.len; j++) {
				if (conns.items[j].marked) continue;
				if (conns.items[j].a == a1->items[i] ||
					conns.items[j].b == a1->items[i]) {
					da_append(a2, conns.items[j].a);
					da_append(a2, conns.items[j].b);
					add_unique(circ, conns.items[j].a);
					add_unique(circ, conns.items[j].b);
					conns.items[j].marked = true;
				}
			}
		}
		printf("readded: ");
		for (uint32_t i = 0; i < a2->len; i++) {
			printf("%u ", a2->items[i]);
		}
		putchar('\n');
		if (a2->len == 0) {
			circ_size = circ->len;
			new_circ = true;
			if (circ_size > c1) {
				c3 = c2; c2 = c1; c1 = circ_size;
			} else if (circ_size < c1 && circ_size > c2) {
				c3 = c2; c2 = circ_size;
			} else if (circ_size < c2 && circ_size > c3) {
				c3 = circ_size;
			}
			da_free(a1);
			da_init(a1);
			da_free(a2);
			da_init(a2);
			da_free(circ);
			da_init(circ);
			continue;
		}
		tmp = a1;
		a1 = a2;
		a2 = tmp;
		da_free(a2);
		da_init(a2);
	}

	res = c1 * c2 * c3;
	printf("part 1 answer: %lu\n", res);
	free(a1);
	free(a2);
	return;
}

/*
void part2(void* inp) {
	input_t* input = (input_t*)inp;
	uint64_t res = 0;

	// solution...

	printf("part 2 answer: %u\n", res);
	return;
}
*/

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

	// time = time_function(part2, &input);
	// printf("took %.10lf seconds to run part 2\n", time);

	da_free(&(input));

	return 0;
}