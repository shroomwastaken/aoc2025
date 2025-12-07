#include "../common.h"

da_struct(char*, input_t);

int gather_input(const char* filename, input_t* out) {
	FILE* f_in = fopen(filename, "r");
	if (f_in == NULL) { return 1; }

	da_init(out);

	char* s;
	while (fscanf(f_in, "%ms", &s) == 1) {
		da_append(out, s);
	}

	fclose(f_in);
	return 0;
}

// r and c are row and column of beam start
uint64_t recur(input_t* lines, uint32_t r, uint32_t c) {
	uint32_t nr = r;
	uint64_t count = 0; bool has_split = false;
	while (nr < lines->len) {
		if (lines->items[nr][c] == '|') break;
		if (lines->items[nr][c] != '^') {
			lines->items[r][c] = '|';
		} else {
			if (lines->items[nr][c-1] != '|') {
				count += recur(lines, nr, c - 1) + 1;
				has_split = true;
			}
			if (lines->items[nr][c+1] != '|') {
				count += recur(lines, nr, c + 1);
				if (!has_split) count += 1;
			}
			break;
		}
		nr++;
	}
	return count;
}

void part1(void* inp) {
	input_t* input = (input_t*)inp;
	uint64_t res = 0;

	// find col of S
	uint64_t scol = (uint64_t)(strchr(input->items[0], 'S') - input->items[0]);
	res = recur(input, 1, scol);

	printf("part 1 answer: %lu\n", res);
	return;
}

void part2(void* inp) {
	input_t* input = (input_t*)inp;
	uint64_t res = 0;

	uint64_t* p = calloc(input->len, sizeof(uint64_t*));

	// find col of S
	uint64_t scol = (uint64_t)(strchr(input->items[0], 'S') - input->items[0]);
	p[scol] = 1;

	uint32_t r = 1;
	while (r < input->len) {
		for (uint32_t c = 0; c < strlen(input->items[0]); c++) {
			if (input->items[r][c] == '^') {
				p[c - 1] += p[c];
				p[c + 1] += p[c];
				p[c] = 0;
			}
		}
		r++;
	}

	for (uint32_t i = 0; i < strlen(input->items[0]); i++) res += p[i];

	free(p);

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

	for (uint32_t i = 0; i < input.len; i++) {
		free(input.items[i]);
	}
	da_free(&(input));

	return 0;
}