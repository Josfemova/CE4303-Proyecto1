#ifndef PARSE_H
#define PARSE_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

struct arguments {
	// Porcentajes expresados como número entre 0-1
	float percent_cortex;
	float percent_qsys;
};

struct arguments parse_args(int argc, char **argv);

#endif
