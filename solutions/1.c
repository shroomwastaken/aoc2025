#include "../common.h"

da_struct(char, dirs_t)
da_struct(uint16_t, rots_t)

typedef struct {
	dirs_t dirs;
	rots_t rots;
} input_t;

int gather_input(const char* filename, input_t* out) {
	FILE* f_in = fopen(filename, "r");
	if (f_in == NULL) { return 1; }

	da_init(&(out->dirs));
	da_init(&(out->rots));

	char dir; uint16_t rot;
	while (fscanf(f_in, "%c%hu ", &dir, &rot) == 2) {
		da_append(&(out->dirs), dir);
		da_append(&(out->rots), rot);
	}

	fclose(f_in);
	return 0;
}

void part1(void* inp) {
	input_t* input = (input_t*)inp;
	uint64_t res = 0;
	int16_t code = 50;

	int8_t mult = 1;
	for (unsigned int i = 0; i < input->dirs.len; i++) {
		if (input->dirs.items[i] == 'L') mult = -1;
		else mult = 1;
		code += mult * (input->rots.items[i] % 100);
		if (code >= 100) code -= 100;
		if (code < 0) code = 100 + code;
		if (code == 0) res++;
	}

	printf("part 1 answer: %ld\n", res);
	return;
}

// really ugly but it works
void part2(void* inp) {
	input_t* input = (input_t*)inp;
	uint64_t res = 0;
	int16_t code = 50;
	int16_t prev_code = code;
	bool here = false;

	int8_t mult = 1;
	for (unsigned int i = 0; i < input->dirs.len; i++) {
		prev_code = code;
		here = false;
		if (input->dirs.items[i] == 'L') mult = -1;
		else mult = 1;

		code += mult * input->rots.items[i];

		while (code > 100) { code -= 100; res++; }
		while (code < 0) { here = true; code += 100; res++; }

		if (prev_code == 0 && here) res--;
		if (code == 100) code -= 100;
		if (code == 0) res++;
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

	da_free(&(input.dirs));
	da_free(&(input.rots));

	return 0;
}