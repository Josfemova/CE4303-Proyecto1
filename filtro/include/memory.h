#ifndef MEMORY_H
#define MEMORY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "parse.h"

struct shared_header {
  int image_w;
  int image_h;
  bool image_copy_done;
  int decrypt_px_count;
  bool decrypt_done;
  // nios comienza a procesar desde pixel 0 hasta hps_start
  int filter_hps_start;
  bool nios_filter_done;
  bool hps_filter_done;
};

extern volatile uint8_t *sdram; // shared memory

/// 64MiB
#define SDRAM_SIZE 0x4000000

#define HEADER_SECTOR 0x0
#define DECRYPTED_IMAGE_SECTOR sizeof(struct shared_header)
// 40MiB para descifrar
#define FILTERED_IMAGE_SECTOR DECRYPTED_IMAGE_SECTOR + 0x2800000

void init_memory(struct arguments args);

void free_memory();

struct shared_header *get_header();

void save_filtered_image();

#endif
