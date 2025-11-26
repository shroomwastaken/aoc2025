#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <stdbool.h>

typedef void (*funcptr_t)(void*);

double time_function(funcptr_t f, void* data);

#endif // COMMON_H