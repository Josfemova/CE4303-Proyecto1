#ifndef TYPES_H_
#define TYPES_H_

#include <inttypes.h>
#include <stdbool.h>

typedef uintptr_t usize;
typedef intptr_t isize;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

#define MAGIC_HANDSHAKE_MASK 0x420
#define HANDSHAKE_VAL_CALC(offset) (offset | MAGIC_HANDSHAKE_MASK)
#define HANDSHAKE_CHECK(offset, read_val) \
  (read_val == (offset | MAGIC_HANDSHAKE_MASK))
#define MAX_RES (4096 * 2160)
typedef struct {
  // de-encrypted in place
  u32 image_encrypted[MAX_RES];
  u8 image_filtered[MAX_RES];
  u32 image_w;
  u32 image_h;
  bool image_copy_done;
  int decrypt_px_count;
  bool decrypt_done;
  // nios comienza a procesar desde pixel 0 hasta hps_start
  int filter_hps_start;
  bool nios_filter_done;
  bool hps_filter_done;
  char message[20];
} shared_data_t;

#endif /* TYPES_H_ */