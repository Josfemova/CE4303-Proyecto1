#ifndef FILTER_H
#define FILTER_H

#include <stddef.h>
#include <stdint.h>

void apply_filter(uint8_t *pixels, uint8_t *out_buf, size_t num_pixels,
                  size_t image_w, size_t image_h);

#endif
