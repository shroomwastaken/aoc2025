#include "../common.h"
#include <math.h>

da_struct(uint16_t, u16da_t)
da_struct(u16da_t, buttons_t)
da_struct(char, cda_t)

typedef struct {
	cda_t lights;
	buttons_t buttons;
	u16da_t joltage;
} machine_t;

da_struct(machine_t, input_t)

int gather_input(const char* filename, input_t* out) {
	FILE* f_in = fopen(filename, "r");
	if (f_in == NULL) { return 1; }

	da_init(out);
	cda_t lights = {0};
	buttons_t buttons = {0};
	u16da_t u16da = {0};
	machine_t machine = {0};

	while (fgetc(f_in) == '[') {
		da_init(&(lights));
		char c;
		while ((c = fgetc(f_in)) != ']')
			da_append(&(lights), c);
		machine.lights = lights;

		fgetc(f_in); // space
		c = fgetc(f_in); // rparen
		da_init(&(buttons));
		uint16_t num;
		while (c != '\n' && c != EOF) {
			da_init(&(u16da));
			fscanf(f_in, "%hu", &num);
			da_append(&(u16da), num);
			while ((c = fgetc(f_in)) == ',') {
				fscanf(f_in, "%hu", &num);
				da_append(&(u16da), num);
			}
			if (c == ')') {
				da_append(&(buttons), u16da);
				fgetc(f_in); // space
				fgetc(f_in); // rparen
			}
			else if (c == '}') {
				machine.joltage = u16da;
				c = fgetc(f_in); // \n or EOF
			}
		}
		machine.buttons = buttons;
		da_append(out, machine);
	}

	fclose(f_in);
	return 0;
}

void apply_button(cda_t* lights, u16da_t* button) {
	char c;
	for (uint32_t i = 0; i < button->len; i++) {
		c = lights->items[button->items[i]];
		lights->items[button->items[i]] = c == '.' ? '#' : '.';
	}
}

// assumes l1.len == l2.len
bool check_lights(cda_t* l1, cda_t* l2) {
	return memcmp(l1->items, l2->items, l1->len) == 0;
}

// c = buttons pressed
// lp == last pressed button
// mc = minimum encountered c
uint64_t recur(cda_t* cur, cda_t* target, buttons_t buttons, uint64_t c, uint32_t lp, uint32_t mc) {
	if (c > buttons.len) return 100000;
	if (c >= mc) return 100000;
	if (check_lights(cur, target)) return c;

	// we didn't solve it, recur with any other different button press
	uint64_t mr = 0;
	for (uint32_t i = 0; i < buttons.len; i++) {
		if (i == lp) continue;
		apply_button(cur, &buttons.items[i]);
		mr = recur(cur, target, buttons, c + 1, i, mc);
		if (mr < mc) mc = mr;
		apply_button(cur, &buttons.items[i]);
	}

	return mc;
}

void make_new_lights(cda_t* n, uint32_t len) {
	da_init(n);
	for (uint32_t i = 0; i < len; i++) da_append(n, '.');
}

void part1(void* inp) {
	input_t* input = (input_t*)inp;
	uint64_t res = 0;

	cda_t l = {0};
	for (uint32_t i = 0; i < input->len; i++) {
		make_new_lights(&l, input->items[i].lights.len);
		res += recur(&l, &input->items[i].lights, input->items[i].buttons, 0, -1, 10000);
		da_free(&(l));
	}

	printf("part 1 answer: %lu\n", res);
	return;
}

da_struct(uint32_t, u32da_t);

// do NOT use 1e-14
// this was the last thing i needed to change to get the right answer
#define EPSILON 1e-12

void swap_rows(double** A, uint32_t r1, uint32_t r2, uint32_t cols) {
	for (uint32_t j = 0; j <= cols; j++) {
		double tmp = A[r1][j];
		A[r1][j] = A[r2][j];
		A[r2][j] = tmp;
	}
}

// convert matrix to reduced row echelon form
void rref(double** A, uint32_t rows, uint32_t cols) {
	uint32_t lead = 0;
	for (uint32_t r = 0; r < rows && lead < cols; lead++) {
		uint32_t pivot = r;
		for (uint32_t i = r; i < rows; i++) {
			if (fabs(A[i][lead]) > fabs(A[pivot][lead])) pivot = i;
		}

		// encountered a zeroed out column
		if (fabs(A[pivot][lead]) < EPSILON) continue;

		swap_rows(A, r, pivot, cols);

		// normalize row, leading element is now 1
		double div = A[r][lead];
		for (uint32_t j = 0; j <= cols; j++) {
			A[r][j] /= div;
		}

		for (uint32_t i = 0; i < rows; i++) {
			if (i == r) continue;
			double factor = A[i][lead];
			for (uint32_t j = 0; j <= cols; j++) {
				A[i][j] -= factor * A[r][j];
			}
		}

		r++;
	}
}

// checks if element is a pivot
// (if its == 1 and all elements above and below == 0
bool is_pivot(double** A, uint32_t i, uint32_t j, uint32_t rows) {
	if (fabs(A[i][j] - 1.0) > EPSILON) return false;

	for (uint32_t k = 0; k < rows; k++) {
		if (k != i && fabs(A[k][j]) > EPSILON) return false;
	}

	return true;
}

// finds indices of free variables and returns their count
uint32_t find_free_vars(double** A, uint32_t rows, uint32_t cols, uint32_t* free_vars) {
	uint32_t pivot_cols[cols];
	uint32_t rank = 0;

	// find pivot columns
	for (uint32_t i = 0; i < rows; i++) {
		for (uint32_t j = 0; j < cols; j++) {
			if (is_pivot(A, i, j, rows)) {
				pivot_cols[rank++] = j;
				break;
			}
		}
	}

	uint32_t free_count = 0;
	for (uint32_t j = 0; j < cols; j++) {
		bool p = false;
		for (uint32_t k = 0; k < rank; k++) {
			if (pivot_cols[k] == j) {
				p = true; break;
			}
		}
		if (!p) {
			free_vars[free_count] = j;
			free_count++;
		}
	}
	return free_count;
}

#define LIMIT 250

// pretty much just advances a "counter" in base LIMIT
bool next_combo(uint32_t* arr, uint32_t len) {
	arr[0]++;
	uint32_t i = 0;
	while (i < len - 1 && arr[i] == LIMIT) {
		arr[i] = 0;
		arr[i + 1]++;
		i++;
	}
	if (arr[i] == LIMIT) {
		arr[i] = 0;
		return false;
	}
	return true;
}

uint32_t elem_in_arr(uint32_t* arr, uint32_t elem, uint32_t len) {
	for (uint32_t i = 0; i < len; i++) {
		if (arr[i] == elem) return i;
	}
	return -1;
}

void part2(void* inp) {
	input_t* input = (input_t*)inp;
	uint64_t res = 0;

	for (uint32_t m = 0; m < input->len; m++) {
		// construct matrix (A|b)
		// A is a matrix of coefficients,
		// where the coefficients are the amount of times
		// a joltage counter appears in the buttons

		u16da_t joltage = input->items[m].joltage;
		buttons_t buttons = input->items[m].buttons;
		int16_t matrix[joltage.len][buttons.len + 1];
		memset(matrix, 0, joltage.len * (buttons.len + 1) * sizeof(int16_t));
		for (uint32_t i = 0; i < joltage.len; i++) {
			matrix[i][buttons.len] = joltage.items[i];
			for (uint32_t j = 0; j < buttons.len; j++) {
				for (uint32_t k = 0; k < buttons.items[j].len; k++) {
					if (buttons.items[j].items[k] == i) matrix[i][j] = 1;
				}
			}
		}

		// we'll make the matrix use doubles since from this point on
		// things may get ugly
		double** matrixd = malloc(joltage.len * sizeof(double*));
		for (uint32_t i = 0; i < joltage.len; i++) {
			matrixd[i] = malloc((buttons.len + 1) * sizeof(double));
			for (uint32_t j = 0; j < buttons.len + 1; j++) {
				matrixd[i][j] = (double)matrix[i][j];
			}
		}

		// convert matrix to Reduced Row Echelon Form
		rref(matrixd, joltage.len, buttons.len);

		// there's not gonna be more than 10 free variables ok
		uint32_t fvs[10] = {0};
		uint32_t fc = find_free_vars(matrixd, joltage.len, buttons.len, fvs);

		if (fc == 0) {
			// no free variables, the matrix is solved
			double sum = 0;
			for (uint32_t i = 0; i < joltage.len; i++) {
				sum += (uint64_t)round(matrixd[i][buttons.len]);
			}
			res += (uint64_t)round(sum);
		} else {
			// we need to guess the free variables :(
			uint32_t values[fc];
			memset(values, 0, fc*sizeof(uint32_t));
			uint32_t fidx = 0;
			double minsum = 1000000, sum = 0;
			uint32_t border = (joltage.len < buttons.len ? joltage.len : buttons.len);
			do {
				sum = 0;
				bool neg = false;
				for (uint32_t i = 0; i < border; i++) {
					uint32_t col = i;
					// if we're on free variable column
					// advance until we're not
					if ((fidx = elem_in_arr(fvs, i, fc)) != -1) {
						while (col < border && (fidx = elem_in_arr(fvs, col, fc)) != -1) {
							col++;
						}
					}
					double eq_sum = matrixd[i][buttons.len];
					for (uint32_t elem = col + 1; elem < buttons.len; elem++) {
						// add the chosen free variable values to the sum
						// with opposite coefficients
						if ((fidx = elem_in_arr(fvs, elem, fc)) != -1) {
							eq_sum += -matrixd[i][elem] * values[fidx];
						}
					}
					sum += eq_sum;
					// if (eq_sum is negative and NOT -0.0) || (sum is already more than minimum)
					if ((fabs(eq_sum) > EPSILON && eq_sum < 0) || sum > minsum) { neg = true; break; }
				}

				// if sum is negative or fractional we skip
				double iptr;
				if (neg || fabs(modf(sum, &iptr)) > EPSILON) { continue; }

				// add the free variable values
				for (uint32_t i = 0; i < fc; i++) { sum += values[i]; }

				if (sum < minsum) { minsum = sum; }
			} while (next_combo(values, fc));
			res += (uint64_t)round(minsum);
		}

		for (uint32_t i = 0; i < joltage.len; i++) {
			free(matrixd[i]);
		}
		free(matrixd);
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
		for (uint32_t j = 0; j < input.items[i].buttons.len; j++) {
			da_free(&(input.items[i].buttons.items[j]));
		}
		da_free(&(input.items[i].buttons));
		da_free(&(input.items[i].joltage));
		da_free(&(input.items[i].lights));
	}
	da_free(&(input));

	return 0;
}