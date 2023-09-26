#include "process.h"
#include "filter.h"
#include "memory.h"
#include "util.h"

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
  } else if (soft_filter_px_count > 0) {
    // Si ya empezó filtrado no esperar
    num_pixels_to_filter = unfiltered_decrypted_px_count;
  }

  apply_filter((uint8_t *)sdram + DECRYPTED_IMAGE_SECTOR + soft_filter_px_count,
               (uint8_t *)sdram + FILTERED_IMAGE_SECTOR + soft_filter_px_count,
               num_pixels_to_filter, header->image_w, header->image_h);
  soft_filter_px_count += num_pixels_to_filter;

  // si ya llego a filter_hps_start deja de filtrar y apaga el
  // ISR. Setea nios_filter_done
  if (soft_filter_px_count == header->filter_hps_start) {
    header->nios_filter_done = true;
  }
}

static int hard_filter_px_count = 0;
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
  // si es automático descifra de un solo, termina loop y pasa a
  // siguiente pixel condición de salida: decrypt_px_count = width
  // * height

  // setear decrypt done
}
