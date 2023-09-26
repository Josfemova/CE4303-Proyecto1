#ifndef FILTER_H
#define FILTER_H

#include <stddef.h>
#include <stdint.h>

void apply_filter(uint8_t *image, size_t image_w, size_t image_h,
                  size_t start_idx, size_t num_pixels);

#endif
