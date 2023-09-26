#include "memory.h"

#include <FreeImage.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ORIGINAL_IMAGE "test_images/imagen_descifrada.png"

volatile uint8_t *sdram; // shared memory

void load_data(struct arguments args);

void init_memory(struct arguments args) {
  sdram = malloc(SDRAM_SIZE * sizeof(uint32_t)); // 64 MB
  memset((void *)sdram, 0, SDRAM_SIZE * sizeof(uint32_t));
  load_data(args);
}

// Helper function to load image header and unciphered image during init
void load_data(struct arguments args) {

  struct shared_header header = {
      .image_copy_done = true,
      .decrypt_px_count = 0,
      .decrypt_done = false,
      .nios_filter_done = false,
      .hps_filter_done = false,
  };

  FIBITMAP *dib;

  FREE_IMAGE_FORMAT image_format = FreeImage_GetFileType(ORIGINAL_IMAGE, 0);

  // Load image
  if ((image_format != FIF_UNKNOWN) &&
      FreeImage_FIFSupportsReading(image_format)) {

    dib = FreeImage_Load(image_format, ORIGINAL_IMAGE, 0);
  } else {
    fprintf(stderr, "Error leyendo imagen descifrada en " ORIGINAL_IMAGE);
    exit(1);
  }

  if (dib == NULL) {
    fprintf(stderr, "Error leyendo imagen descifrada");
    exit(1);
    return;
  }

  if (FreeImage_GetBPP(dib) != 8) {
    printf("CONVERTING");
    FreeImage_ConvertToGreyscale(dib);
  }

  unsigned width = FreeImage_GetWidth(dib);
  unsigned height = FreeImage_GetHeight(dib);
  printf("width: %d\n", width);
  printf("height: %d\n", height);

  header.image_w = width;
  header.image_h = height;
  header.filter_hps_start = width * height * args.percent_qsys + 1;

  *(struct shared_header *)sdram = header;

  BYTE *pixels = FreeImage_GetBits(dib);

  for (size_t i = 0; i < width * height; i++) {
    sdram[DECRYPTED_IMAGE_SECTOR + i] = pixels[i];
  }
}

struct shared_header *get_header() { return (struct shared_header *)sdram; }

void save_filtered_image() {
  BYTE *pixels = (BYTE *)sdram + DECRYPTED_IMAGE_SECTOR;
  struct shared_header *header = get_header();

  FIBITMAP *dib =
      FreeImage_ConvertFromRawBits(pixels, header->image_w, header->image_h,
                                   header->image_w, 8, 0, 0, 0, FALSE);

  FreeImage_Save(FIF_PNG, dib, "resultado.png", 0);
}

void free_memory() { free((void *)sdram); }
