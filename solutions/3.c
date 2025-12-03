#include "../common.h"

da_struct(unsigned char, jolt_t)
da_struct(jolt_t, input_t)

int gather_input(const char* filename, input_t* out) {
	FILE* f_in = fopen(filename, "r");
	if (f_in == NULL) { return 1; }

	da_init(out);

	jolt_t jolts;
	da_init(&(jolts));
	char c;
	while ((c = (char)fgetc(f_in)) != EOF) {
		while (isdigit(c)) {
			da_append(&(jolts), c - '0');
			c = fgetc(f_in);
		}
		da_append(out, jolts);
		da_init(&(jolts));
	}

	fclose(f_in);
	return 0;
}

void part1(void* inp) {
	input_t* input = (input_t*)inp;
	uint64_t res = 0;

	printf("inp len: %u\n", input->len);
	printf("jolt0 len: %u\n", input->items[0].len);

	printf("part 1 answer: %lu\n", res);
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

	for (uint32_t i = 0; i < input.len; i++) {
		da_free(&(input.items[i]));
	}
	da_free(&(input));

	return 0;
}