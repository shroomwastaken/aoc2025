#include "common.h"
#include <time.h>

double time_function(funcptr_t f, void* data) {
	clock_t start, end;

	start = clock();
	f(data);
	end = clock();

	return ((double)(end - start)) / CLOCKS_PER_SEC;
}
