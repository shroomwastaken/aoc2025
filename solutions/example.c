#include "../common.h"

/*
	this is an example of what a solution would look like
	here the input is just 1 number per line
	part 1 calculates the sum of the numbers
	part 2 calculates the product of the numbers
*/

typedef struct {
	uint32_t arr_len;
	uint32_t arr_size;
	uint32_t* arr;
} input_t;

int gather_input(const char* filename, input_t* out) {
	FILE* f_in = fopen(filename, "r");
	if (f_in == NULL) { return 1; }

	out->arr_size = 2;
	out->arr_len = 0;
	out->arr = malloc(out->arr_size * sizeof(int32_t));
	while (fscanf(f_in, "%d", &out->arr[out->arr_len]) == 1) {
		out->arr_len++;
		if (out->arr_len >= out->arr_size) {
			out->arr_size *= 2;
			out->arr = realloc(out->arr, out->arr_size * sizeof(int32_t));
		}
	}

	fclose(f_in);
	return 0;
}

void part1(void* inp) {
	input_t* input = (input_t*)inp;
	uint32_t res = 0;
	for (uint32_t i = 0; i < input->arr_len; i++) {
		res += input->arr[i];
	}

	printf("%u\n", res);
	return;
}

void part2(void* inp) {
	input_t* input = (input_t*)inp;
	uint32_t res = 1;
	for (uint32_t i = 0; i < input->arr_len; i++) {
		res *= input->arr[i];
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

	free(input.arr);
	return 0;
}