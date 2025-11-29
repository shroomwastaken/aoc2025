#include "../common.h"

/*
	this is an example of what a solution would look like
	here the input is just 1 number per line
	part 1 calculates the sum of the numbers
	part 2 calculates the product of the numbers
*/

da_struct(uint32_t, input_t)

int gather_input(const char* filename, input_t* out) {
	FILE* f_in = fopen(filename, "r");
	if (f_in == NULL) { return 1; }

	da_init(out);

	uint32_t item;

	while (fscanf(f_in, "%d", &item) == 1) {
		da_append(out, item);
	}

	fclose(f_in);
	return 0;
}

void part1(void* inp) {
	input_t* input = (input_t*)inp;
	uint32_t res = 0;
	for (uint32_t i = 0; i < input->len; i++) {
		res += input->items[i];
	}

	printf("%u\n", res);
	return;
}

void part2(void* inp) {
	input_t* input = (input_t*)inp;
	uint32_t res = 1;
	for (uint32_t i = 0; i < input->len; i++) {
		res *= input->items[i];
	}

	printf("%u\n", res);
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

	free(input.items);
	return 0;
}