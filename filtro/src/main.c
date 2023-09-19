#include <stdio.h>

#include "parse.h"
#include "util.h"

int main(int argc, char **argv) {

  struct arguments args = parse_args(argc, argv);

  printf("Hello World!\r\n");
  printf("%% Cortex: %f\r\n", args.percent_cortex);
  printf("%% Qsys: %f\r\n", args.percent_qsys);

  return 0;
}
