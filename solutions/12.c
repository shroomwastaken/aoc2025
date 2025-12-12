#include "../common.h"

// there are 6 in my input,
// if you want it to work adjust accordingly
#define NUM_PRESENTS 6

typedef struct {
	uint32_t dim_x, dim_y;
	uint32_t presents[NUM_PRESENTS];
} plane_t;

da_struct(plane_t, planes_t);

typedef struct {
	uint32_t amt_of_filled[NUM_PRESENTS];
	planes_t planes;
} input_t;

int gather_input(const char* filename, input_t* out) {
	FILE* f_in = fopen(filename, "r");
	if (f_in == NULL) { return 1; }

	fseek(f_in, 0, SEEK_END);
	uint64_t f_len = ftell(f_in);
	fseek(f_in, 0, SEEK_SET);

	char* s = calloc(f_len + 1, sizeof(char)), *s_sptr;
	fread(s, f_len, 1, f_in);

	plane_t plane = {0};
	planes_t planes = {0}; da_init(&(planes));
	char* line, *line_sptr;
	line = strtok_r(s, "\n", &s_sptr);
	uint32_t p_idx;
	do {
		if (strchr(line, 'x') == NULL) {
			sscanf(line, "%u:", &p_idx);
			for (uint32_t i = 0; i < 3; i++) {
				line = strtok_r(NULL, "\n", &s_sptr);
				for (uint32_t j = 0; j < 3; j++) {
					if (line[j] == '#')
						out->amt_of_filled[p_idx]++;
				}
			}
		} else {
			sscanf(line, "%ux%u", &plane.dim_x, &plane.dim_y);
			strtok_r(line, ": ", &line_sptr);
			line = strtok_r(NULL, " ", &line_sptr);
			for (uint32_t i = 0; i < NUM_PRESENTS; i++) {
				sscanf(line, "%u", &plane.presents[i]);
				line = strtok_r(NULL, " ", &line_sptr);
			}
			da_append(&(planes), plane);
		}
	} while ((line = strtok_r(NULL, "\n", &s_sptr)) != NULL);

	out->planes = planes;

	free(s);
	fclose(f_in);
	return 0;
}

void part1(void* inp) {
	input_t* input = (input_t*)inp;
	uint64_t res = 0;

	uint32_t total = 0;
	for (uint32_t i = 0; i < input->planes.len; i++) {
		total = 0;
		plane_t plane = input->planes.items[i];
		for (uint32_t j = 0; j < NUM_PRESENTS; j++) {
			total += plane.presents[j]*input->amt_of_filled[j];
		}
		if (total < plane.dim_x * plane.dim_y) res++;
	}

	printf("part 1 answer: %lu\n", res);
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

	printf("there isn't a part 2!\n");

	da_free(&(input.planes));

	return 0;
}