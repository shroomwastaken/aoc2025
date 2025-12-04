#include "../common.h"

da_struct(char*, input_t)

int gather_input(const char* filename, input_t* out) {
	FILE* f_in = fopen(filename, "r");
	if (f_in == NULL) { return 1; }

	da_init(out);

	char* line;
	while (fscanf(f_in, "%ms", &line) == 1) {
		da_append(out, line);
	}

	fclose(f_in);
	return 0;
}

// change of row, change of column
int32_t directions[8][2] = {
	{-1, -1}, {-1, 0}, {-1, 1},
	{0, -1},           {0, 1},
	{1, -1},  {1, 0},  {1, 1}
};

void part1(void* inp) {
	input_t* input = (input_t*)inp;
	uint64_t res = 0;

	uint32_t rowlen = strlen(input->items[0]), neighbors;
	int32_t nr, nc;
	for (int32_t r = 0; r < input->len; r++) {
		for (int32_t c = 0; c < rowlen; c++) {
			neighbors = 0;
			if (input->items[r][c] == '.') continue;
			for (int32_t d = 0; d < 8; d++) {
				nr = directions[d][0]; nc = directions[d][1];
				if (r + nr >= 0 && r + nr < input->len &&
					c + nc >= 0 && c + nc < rowlen) {
					if (input->items[r + nr][c + nc] == '@')
						neighbors++;
				}
			}
			if (neighbors < 4) res++;
		}
	}

	printf("part 1 answer: %lu\n", res);
	return;
}

void part2(void* inp) {
	input_t* input = (input_t*)inp;
	uint64_t res = 0;

	uint32_t rowlen = strlen(input->items[0]), neighbors;
	int32_t nr, nc;
	bool have_removed = true;
	while (have_removed) {
		have_removed = false;

		// find neighbors just like part1
		// (but count Xs as rolls that are still there)
		for (int32_t r = 0; r < input->len; r++) {
			for (int32_t c = 0; c < rowlen; c++) {
				neighbors = 0;
				if (input->items[r][c] == '.') continue;
				for (int32_t d = 0; d < 8; d++) {
					nr = directions[d][0]; nc = directions[d][1];
					if (r + nr >= 0 && r + nr < input->len &&
						c + nc >= 0 && c + nc < rowlen) {
						if (input->items[r + nr][c + nc] == '@' ||
							input->items[r + nr][c + nc] == 'x') {
							neighbors++;
						}
					}
				}
				if (neighbors < 4) {
					res++;
					// this roll will be taken
					input->items[r][c] = 'x';
					have_removed = true;
				}
			}
		}

		// remove Xs
		for (int32_t r = 0; r < input->len; r++) {
			for (int32_t c = 0; c < rowlen; c++) {
				if (input->items[r][c] == 'x') {
					input->items[r][c] = '.';
				}
			}
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

	for (uint32_t i = 0; i < input.len; i++)
		free(input.items[i]);
	da_free(&(input));

	return 0;
}