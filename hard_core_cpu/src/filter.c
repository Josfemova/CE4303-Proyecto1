#include "filter.h"

#include <math.h>
#include <stdbool.h>

static uint32_t filter[9] = {
    0, 1, 0, 1, -4, 1, 0, 1, 0,
};

uint8_t filter_pixel(uint32_t *image, size_t idx, size_t image_w,
                     size_t image_h) {
  size_t y = idx / image_w;
  size_t x = idx - y * image_w;

  bool left_border = (x == 0);
  bool right_border = (x == image_w - 1);
  bool top_border = (y == 0);
  bool bottom_border = (y == image_h - 1);

  static uint32_t pixels[9] = {0};

  if (top_border) {
    // x 0 0
    // 0 0 0
    // 0 0 0
    if (left_border) {
      pixels[0] = image[idx];
    } else {
      pixels[0] = image[idx - 1];
    }

    // 0 x 0
    // 0 0 0
    // 0 0 0
    pixels[1] = image[idx];

    // 0 0 x
    // 0 0 0
    // 0 0 0
    if (right_border) {
      pixels[2] = image[idx];
    } else {
      pixels[2] = image[idx + 1];
    }
  } else {
    // x 0 0
    // 0 0 0
    // 0 0 0
    if (left_border) {
      pixels[0] = image[idx - image_w];
    } else {
      pixels[0] = image[idx - image_w - 1];
    }

    // 0 x 0
    // 0 0 0
    // 0 0 0
    pixels[1] = image[idx - image_w];

    // 0 0 x
    // 0 0 0
    // 0 0 0
    if (right_border) {
      pixels[2] = image[idx - image_w];
    } else {
      pixels[2] = image[idx - image_w + 1];
    }
  }

  // 0 0 0
  // x 0 0
  // 0 0 0
  if (left_border) {
    pixels[3] = image[idx];
  } else {
    pixels[3] = image[idx - 1];
  }

  // 0 0 0
  // 0 x 0
  // 0 0 0
  pixels[4] = image[idx];

  // 0 0 0
  // 0 0 x
  // 0 0 0
  if (right_border) {
    pixels[5] = image[idx];
  } else {
    pixels[5] = image[idx + 1];
  }

  if (bottom_border) {
    // 0 0 0
    // 0 0 0
    // x 0 0
    if (left_border) {
      pixels[6] = image[idx];
    } else {
      pixels[6] = image[idx - 1];
    }

    // 0 0 0
    // 0 0 0
    // 0 x 0
    pixels[7] = image[idx];

    // 0 0 0
    // 0 0 0
    // 0 0 x
    if (right_border) {
      pixels[8] = image[idx];
    } else {
      pixels[8] = image[idx + 1];
    }
  } else {
    // 0 0 0
    // 0 0 0
    // x 0 0
    if (left_border) {
      pixels[6] = image[idx + image_w];
    } else {
      pixels[6] = image[idx + image_w - 1];
    }

    // 0 0 0
    // 0 0 0
    // 0 x 0
    pixels[7] = image[idx + image_w];

    // 0 0 0
    // 0 0 0
    // 0 0 x
    if (right_border) {
      pixels[8] = image[idx + image_w];
    } else {
      pixels[8] = image[idx + image_w + 1];
    }
  }

  uint32_t result = 0;
  for (size_t i = 0; i < 9; i++) {
    result += pixels[i] * filter[i];
  }

  return (uint8_t)result;
}

void apply_filter(uint32_t *image, uint8_t *out_buf, size_t start_idx,
                  size_t num_pixels, size_t image_w, size_t image_h) {
  for (size_t i = start_idx; i < (start_idx + num_pixels); i++) {
    out_buf[i] = filter_pixel(image, i, image_w, image_h);
  }
}
