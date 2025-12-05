#include "../common.h"

typedef struct {
	uint64_t start;
	uint64_t end;
} range_t;

da_struct(range_t, ranges_t)

da_struct(uint64_t, ingredients_t)

typedef struct {
	ranges_t rs;
	ingredients_t is;
} input_t;

int gather_input(const char* filename, input_t* out) {
	FILE* f_in = fopen(filename, "r");
	if (f_in == NULL) { return 1; }

	da_init(&(out->rs));
	da_init(&(out->is));

	uint64_t num, num2;
	range_t range;
	while (fscanf(f_in, " %lu-%lu", &num, &num2) == 2) {
		range.start = num;
		range.end = num2;
		da_append(&(out->rs), range);
	}

	// the last fscanf assigned the first ingredient to num
	da_append(&(out->is), num);

	while (fscanf(f_in, " %lu ", &num) == 1) {
		da_append(&(out->is), num);
	}

	fclose(f_in);
	return 0;
}

void part1(void* inp) {
	input_t* input = (input_t*)inp;
	uint64_t res = 0;

	bool flag = false;
	for (uint64_t i = 0; i < input->is.len; i++) {
		flag = false;
		for (uint64_t j = 0; j < input->rs.len; j++) {
			if (input->is.items[i] >= input->rs.items[j].start &&
				input->is.items[i] <= input->rs.items[j].end) {
				flag = true;
				break;
			}
		}
		if (flag) res++;
	}

	printf("part 1 answer: %lu\n", res);
	return;
}

int cmp_starts(const void* a, const void* b) {
	range_t* ra = (range_t*)a;
	range_t* rb = (range_t*)b;

	if (ra->start < rb->start) return -1;
	else if (ra->start > rb->start) return 1;
	return 0;
}

void part2(void* inp) {
	input_t* input = (input_t*)inp;
	uint64_t res = 0;

	qsort(input->rs.items, input->rs.len, sizeof(range_t), cmp_starts);

	// adjust start and end positions
	// there's definitely a more efficient way to do this
	// but idrc at this point in time
	for (uint64_t i = 0; i < input->rs.len-1; i++) {
		if (input->rs.items[i].end >= input->rs.items[i + 1].start) {
			input->rs.items[i+1].start = input->rs.items[i].end + 1;
			if (input->rs.items[i].end >= input->rs.items[i + 1].end) {
				input->rs.items[i + 1].end = input->rs.items[i + 1].start - 1;
			}
		}
	}

	for (uint64_t i = 0; i < input->rs.len; i++) {
		if (input->rs.items[i].end >= input->rs.items[i].start) {
			res += input->rs.items[i].end - input->rs.items[i].start + 1;
		}
	}

	printf("part 2 answer: %lu\n", res);
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

	da_free(&(input.rs));
	da_free(&(input.is));

	return 0;
}