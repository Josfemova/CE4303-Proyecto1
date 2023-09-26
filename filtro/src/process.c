#include "process.h"
#include "filter.h"
#include "memory.h"
#include "util.h"
#include <pthread.h>
#include <unistd.h>

static int soft_filter_px_count = 0;
void timer_isr_soft_core() {
  struct shared_header *header = get_header();

  // si hay más de dos filas descifradas, aplica filtro hasta agotar los pixeles
  // descifrados que se pueden filtrar
  int num_pixels_to_filter = 0;

  int unfiltered_decrypted_px_count =
      header->decrypt_px_count - soft_filter_px_count;

  if (soft_filter_px_count + unfiltered_decrypted_px_count >
      header->filter_hps_start) {
    // Truncar pixeles a procesar con el filter_hps_start
    unfiltered_decrypted_px_count =
        header->filter_hps_start - soft_filter_px_count;
  }

  // Esperar a que hayan 2 filas al inicio
  if (soft_filter_px_count == 0 &&
      (unfiltered_decrypted_px_count > 2 * header->image_w)) {
    num_pixels_to_filter = unfiltered_decrypted_px_count;
  } else if ((soft_filter_px_count > 0) || header->decrypt_done) {
    // Si ya empezó filtrado no esperar
    num_pixels_to_filter = unfiltered_decrypted_px_count;
  }

  apply_filter((uint8_t *)sdram + DECRYPTED_IMAGE_SECTOR,
               (uint8_t *)sdram + FILTERED_IMAGE_SECTOR, soft_filter_px_count,
               num_pixels_to_filter, header->image_w, header->image_h);
  soft_filter_px_count += num_pixels_to_filter;

  printf("Nios: pixeles filtrados %d\t%d%%\n", soft_filter_px_count,
         100 * soft_filter_px_count / header->filter_hps_start);

  // si ya llego a filter_hps_start deja de filtrar y apaga el
  // ISR. Setea nios_filter_done
  if (soft_filter_px_count == header->filter_hps_start) {
    header->nios_filter_done = true;
    printf("Cortex FINISH\n");
    // Apagar ISR
    pthread_exit(0);
  }
}

static int hard_filter_px_count = 0;
void proceso_periódico_hard_core() {
  // fijarse en header de programa de sdram
  struct shared_header *header = get_header();

  // hay pixeles pa procesar por parte del HPS?
  if (header->decrypt_px_count < header->filter_hps_start) {
    return;
  }
  // si sí, procesa lo que hay

  // si hay más de dos filas descifradas, aplica filtro hasta agotar los pixeles
  // descifrados que se pueden filtrar
  int num_pixels_to_filter = 0;

  int unfiltered_decrypted_px_count = header->decrypt_px_count -
                                      header->filter_hps_start -
                                      hard_filter_px_count;

  // Asegurar que hayan 2 filas descifradas antes de empezar
  if (hard_filter_px_count == 0 &&
      ((header->filter_hps_start + unfiltered_decrypted_px_count) >
       2 * header->image_w)) {
    num_pixels_to_filter = unfiltered_decrypted_px_count;
  } else if ((hard_filter_px_count > 0) || header->decrypt_done) {
    // Si ya empezó filtrado no esperar
    num_pixels_to_filter = unfiltered_decrypted_px_count;
  }

  apply_filter((uint8_t *)sdram + DECRYPTED_IMAGE_SECTOR,
               (uint8_t *)sdram + FILTERED_IMAGE_SECTOR,
               header->filter_hps_start + hard_filter_px_count,
               num_pixels_to_filter, header->image_w, header->image_h);
  hard_filter_px_count += num_pixels_to_filter;

  printf("Cortex: pixeles filtrados %d\t%d%%\n", hard_filter_px_count,
         100 * hard_filter_px_count /
             (header->image_w * header->image_h - header->filter_hps_start));

  // si ya llego a filter_hps_start deja de filtrar y apaga el
  // ISR. Setea nios_filter_done
  if (header->filter_hps_start + hard_filter_px_count ==
      header->image_w * header->image_h) {
    header->hps_filter_done = true;
    printf("Cortex FINISH\n");
    // Apagar ISR
    pthread_exit(0);
  }
}

void main_loop_soft_core() {
  struct shared_header *header = get_header();

  struct timespec pause_time;
  pause_time.tv_sec = 0;
  pause_time.tv_nsec = 10;

  // loop
  while (header->decrypt_px_count != header->image_w * header->image_h) {
    // (modo automático, luego pongo manual)
    // si es automático descifra de un solo, termina loop y pasa a
    // siguiente pixel condición de salida: decrypt_px_count = width
    // * height
    header->decrypt_px_count++;
    nanosleep(&pause_time, NULL);
  }

  header->decrypt_done = true;
}
