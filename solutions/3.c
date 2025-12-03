#include "../common.h"
#include <math.h> // pow

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

	da_free(&(jolts)); // last jolts isnt needed

	fclose(f_in);
	return 0;
}

void part1(void* inp) {
	input_t* input = (input_t*)inp;
	uint64_t res = 0;

	for (uint32_t i = 0; i < input->len; i++) {
		uint32_t mr = 0, md1, md2;
		for (uint32_t p1 = 0; p1 < input->items[i].len - 1; p1++) {
			for (uint32_t p2 = p1 + 1; p2 < input->items[i].len; p2++) {
				md1 = input->items[i].items[p1];
				md2 = input->items[i].items[p2];
				if (md1*10 + md2 > mr) mr = md1*10 + md2;
			}
		}
		res += mr;
	}

	printf("part 1 answer: %lu\n", res);
	return;
}

void part2(void* inp) {
	input_t* input = (input_t*)inp;
	uint64_t res = 0;

	for (uint32_t i = 0; i < input->len; i++) {
		jolt_t bank = input->items[i];

		// choose last 12 digits
		unsigned char pos[12] = {0};
		for (uint32_t j = bank.len - 12; j < bank.len; j++) {
			pos[j - bank.len + 12] = j;
		}

		int32_t mpos = -1; // maximum position
		// for each of the 12 needed digits
		for (uint32_t j = 0; j < 12; j++) {
			// maximum found value for this digit
			uint32_t maxd = bank.items[pos[j]];
			// loop from current digit (12-j is because they're
			// stored in reverse order) until reaching the position
			// before the previous taken position
			for (int32_t idx = bank.len-(12-j); idx > mpos; idx--) {
				// if we found a better number
				// OR an equal number at a better position
				if (bank.items[idx] >= maxd) {
					maxd = bank.items[idx];
					pos[j] = idx;
				}
			}
			// update best position
			mpos = pos[j];
		}

		// compute joltage
		uint64_t jolt = 0;
		for (uint32_t j = 1; j < 13; j++) {
			jolt += bank.items[pos[j-1]] * powl(10, (12-j));
		}
		res += jolt;
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

	for (uint32_t i = 0; i < input.len; i++) {
		da_free(&(input.items[i]));
	}
	da_free(&(input));

	return 0;
}