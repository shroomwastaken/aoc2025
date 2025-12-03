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

	// choose last 12 digits,
	// maximize position and value of each digit
	for (uint32_t i = 0; i < input->len; i++) {
		jolt_t bank = input->items[i];
		unsigned char pos[12];
		for (uint32_t j = bank.len - 12; j < bank.len; j++) {
			pos[j - bank.len + 12] = j;
			printf("%u: %u ", j - bank.len + 12, j);
		}
		putchar('\n');



		// go through all previous numbers, chose one such that:
		//  1) its highest possible
		//  2) its as far along as possible
		//     (position isn't taken and is more than max(pos))
		uint32_t mpos = 0;
		for (uint32_t j = 0; j < 12; j++) {
			uint32_t maxd = bank.items[12-j];
			for (int32_t idx = bank.len-(12-j)-1; idx >= 0 && idx > mpos; idx--) {
				if (bank.items[idx] >= maxd && idx > mpos) {
					maxd = bank.items[idx];
					mpos = idx;
					pos[j] = idx;
				}
			}
		}

		// compute joltage
		uint64_t jolt = 0;
		for (uint32_t j = 1; j < 13; j++) {
			jolt += bank.items[pos[j-1]] * powl(10, (12-j));
		}
		printf("joltage %u: %lu\n", i, jolt);
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