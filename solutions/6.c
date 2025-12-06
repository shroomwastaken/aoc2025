#include "../common.h"
#include <math.h>

da_struct(uint32_t, line_t)
da_struct(line_t, lines_t)
da_struct(char, op_t)

typedef struct {
	lines_t lines;
	op_t ops;
} input_t;

// parsing is completely different for part 1 and part 2
// so gather_input function is not needed
// parsing is relegated to the part1 and 2 functions

// arg is void* to make it compatible with funcptr_t
void part1(void* filename) {
	char* f_name = (char*)filename;
	uint64_t res = 0;

	input_t input = {0};

	FILE* f_in = fopen(f_name, "r");
	if (f_in == NULL) {
		printf("couldn't open file\n");
		exit(1);
	}

	da_init(&(input.lines));
	da_init(&(input.ops));

	fseek(f_in, 0, SEEK_END);
	uint32_t f_len = ftell(f_in);
	fseek(f_in, 0, SEEK_SET);

	char* s = malloc((f_len + 1)*sizeof(char));
	fread(s, f_len, 1, f_in);

	char* line_saveptr = NULL;
	char* word_saveptr = NULL;
	char* s_line = strtok_r(s, "\n", &line_saveptr);
	line_t line; uint32_t num; char op; bool is_numl;
	da_init(&(line));
	while (s_line != NULL) {
		is_numl = false;
		char* word = strtok_r(s_line, " ", &word_saveptr);
		while (word != NULL) {
			if (isdigit(word[0])) {
				is_numl = true;
				sscanf(word, "%u", &num);
				da_append(&(line), num);
			} else {
				sscanf(word, "%c", &op);
				da_append(&(input.ops), op);
			}
			word = strtok_r(NULL, " ", &word_saveptr);
		}
		if (is_numl) {
			da_append(&(input.lines), line);
			da_init(&(line));
		}
		s_line = strtok_r(NULL, "\n", &line_saveptr);
	}
	da_free(&(line));

	fclose(f_in);
	free(s);

	for (uint32_t i = 0; i < input.lines.items[0].len; i++) {
		char op = input.ops.items[i];
		uint64_t num = op == '+' ? 0 : 1;
		for (uint32_t j = 0; j < input.lines.len; j++) {
			if (op == '+') num += input.lines.items[j].items[i];
			else num *= input.lines.items[j].items[i];
		}
		res += num;
	}

	printf("part 1 answer: %lu\n", res);

	for (uint32_t i = 0; i < input.lines.len; i++) {
		da_free(&(input.lines.items[i]));
	}
	da_free(&(input.lines));
	da_free(&(input.ops));

	return;
}

da_struct(char*, slines_t)

void part2(void* filename) {
	char* f_name = (char*)filename;

	FILE* f_in = fopen(f_name, "r");
	if (f_in == NULL) {
		printf("couldn't open file\n");
		exit(1);
	}

	fseek(f_in, 0, SEEK_END);
	uint64_t f_len = ftell(f_in);
	fseek(f_in, 0, SEEK_SET);

	char* s = malloc((f_len + 1)*sizeof(char));
	fread(s, f_len, 1, f_in);

	slines_t lines;
	da_init(&(lines));

	char* line = strtok(s, "\n");
	while (line != NULL) {
		da_append(&(lines), line);
		line = strtok(NULL, "\n");
	}

	uint64_t res = 0;
	char* numstr = calloc(100, sizeof(char));
	uint64_t idx = 0;
	bool all_spaces = true;
	char* tok; uint64_t eq_res; char op;
	for (int32_t i = strlen(lines.items[0]) - 1; i >= 0; i--) {
		all_spaces = true;
		for (uint32_t l = 0; l < lines.len-1; l++) {
			if (lines.items[l][i] != ' ') all_spaces = false;
			numstr[idx++] = lines.items[l][i];
		}
		numstr[idx++] = ' ';
		if (all_spaces || i == 0) {
			if (i != 0) op = lines.items[lines.len - 1][i + 1];
			else op = lines.items[lines.len - 1][i];
			eq_res = op == '+' ? 0 : 1;
			tok = strtok(numstr, " ");
			while (tok != NULL) {
				if (op == '+') eq_res += atoi(tok);
				else eq_res *= atoi(tok);
				tok = strtok(NULL, " ");
			}
			res += eq_res;
			free(numstr);
			if (i != 0) numstr = calloc(100, sizeof(char));
			idx = 0;
		}
	}

	printf("part 2 answer: %lu\n", res);

	fclose(f_in);
	da_free(&(lines));
	free(s);
	return;
}

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("provide an input file name\n");
		return 1;
	}

	// no gather input function

	double time;

	time = time_function(part1, argv[1]);
	printf("took %.10lf seconds to run part 1\n", time);

	time = time_function(part2, argv[1]);
	printf("took %.10lf seconds to run part 2\n", time);

	return 0;
}