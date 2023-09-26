#include <stdio.h>
#include <unistd.h>

#include "memory.h"
#include "parse.h"
#include "timer.h"
#include "util.h"

void timer_isr() { printf("mik!\n"); }

void timer_isr_soft_core() {
  struct shared_header *header = get_header();
  UNUSED(header);
  // si hay más de dos filas descifradas, aplica filtro hasta agotar los pixeles
  // descifrados que se pueden filtrar

  // si ya llego a filter_hps_start deja de filtrar y apaga el ISR. Setea
  // nios_filter_done
}

void proceso_periódico_hard_core() {
  // fijarse en header de programa de sdram
  struct shared_header *header = get_header();
  UNUSED(header);

  // hay pixeles pa procesar por parte del HPS?
  // si sí, procesa lo que hay

  // si no, si no ha terminado hace yield

  // si ya termino, guarda ambas imagenes a sd
}

void main_loop_soft_core() {
  struct shared_header *header = get_header();
  UNUSED(header);
  // loop
  // chequea switch de modo de descifrado
  // si es manual, descifra hasta ver boton
  // si es automático descifra de un solo, termina loop y pasa a siguiente
  // pixel
  // condición de salida: decrypt_px_count = width * height

  // setear decrypt done
}

int main(int argc, char **argv) {

  struct arguments args = parse_args(argc, argv);

  printf("Hello World!\r\n");
  printf("%% Cortex: %f\r\n", args.percent_cortex);
  printf("%% Qsys: %f\r\n", args.percent_qsys);

  init_memory(args);

  start_timer(timer_isr_soft_core);
  start_timer(proceso_periódico_hard_core);

  main_loop_soft_core();

  save_filtered_image();
  free_memory();
  return 0;
}
