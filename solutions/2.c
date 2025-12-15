#include "../common.h"
#include <math.h> // log10

da_struct(uint64_t, numarr_t)

typedef struct {
	numarr_t r_start;
	numarr_t r_end;
} input_t;

int gather_input(const char* filename, input_t* out) {
	FILE* f_in = fopen(filename, "r");
	if (f_in == NULL) { return 1; }

	da_init(&(out->r_start));
	da_init(&(out->r_end));

	uint64_t num;
	while (fscanf(f_in, "%lu", &num) == 1) {
		fgetc(f_in); // the dash
		da_append(&(out->r_start), num);
		fscanf(f_in, "%lu", &num);
		da_append(&(out->r_end), num);
		fgetc(f_in); // the comma
	}

	fclose(f_in);
	return 0;
}

bool check_num_p1(uint64_t n) {
	uint32_t numchars = (uint32_t)(ceil(log10(n)));
	if (numchars % 2 != 0 || numchars == 0) return false;
	char s[numchars + 1];
	sprintf(s, "%lu", n);
	for (uint32_t i = 0; i < numchars / 2; i++) {
		if (s[i] != s[i + numchars/2]) return false;
	}
	return true;
}

void part1(void* inp) {
	input_t* input = (input_t*)inp;
	uint64_t res = 0;
	for (uint64_t i = 0; i < input->r_start.len; i++) {
		for (uint64_t n = input->r_start.items[i]; n <= input->r_end.items[i]; n++) {
			if (check_num_p1(n)) res += n;
		}
	}

	printf("part 1 answer: %lu\n", res);
	return;
}

bool check_num_p2(uint64_t n) {
	uint32_t numchars = (uint32_t)(ceil(log10(n)));
	if (numchars == 0 || numchars == 1) return false;
	char s[numchars + 1];
	char p[numchars / 2]; // period
	char* substr;
	uint32_t offset = 0;
	sprintf(s, "%lu", n);
	for (uint32_t i = 1; i <= numchars / 2; i++) {
		offset = 0;
		if (numchars % i != 0) continue;
		snprintf(p, i + 1, "%lu", n); // i bytes + \0
		while ((substr = strstr(s + offset, p)) == s + offset && offset < numchars) {
			offset += i;
		}
		if (offset == numchars) return true;
	}
	return false;
}

void part2(void* inp) {
	input_t* input = (input_t*)inp;
	uint64_t res = 0;

	for (uint64_t i = 0; i < input->r_start.len; i++) {
		for (uint64_t n = input->r_start.items[i]; n <= input->r_end.items[i]; n++) {
			if (check_num_p2(n)) res += n;
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

	da_free(&(input.r_start));
	da_free(&(input.r_end));

	return 0;
}