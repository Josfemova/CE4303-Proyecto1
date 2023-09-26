#include <stdio.h>
#include <unistd.h>

#include "memory.h"
#include "parse.h"
#include "process.h"
#include "timer.h"

int main(int argc, char **argv) {

  struct arguments args = parse_args(argc, argv);

  init_memory(args);

  start_timer(timer_isr_soft_core);

  // revisar si le asignaron pixeles al hps
  struct shared_header *header = get_header();

  if (header->filter_hps_start < header->image_w * header->image_h) {
    start_timer(proceso_periódico_hard_core);
  } else {
    header->hps_filter_done = true;
  }

  main_loop_soft_core();

	// esperar a que termine filtrado
  while (!header->nios_filter_done || !header->hps_filter_done) ;

  save_filtered_image();
  free_memory();
  return 0;
}
