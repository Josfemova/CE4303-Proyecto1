#ifndef PARSE_H
#define PARSE_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

struct arguments {
	// Porcentajes expresados como número entre 0-1
	float percent_qsys;
	float percent_cortex;
};

struct arguments parse_args(int argc, char **argv);

#endif
