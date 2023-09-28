#ifndef FILTER_H
#define FILTER_H

#include <stddef.h>
#include <stdint.h>

void apply_filter(uint32_t *image, uint8_t *out_buf, size_t start_idx,
                  size_t num_pixels, size_t image_w, size_t image_h);

#endif
