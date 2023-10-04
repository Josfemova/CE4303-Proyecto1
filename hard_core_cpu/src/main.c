#include <fcntl.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#include "filter.h"
#include "misc.h"
#include "types.h"

time_t seconds;
volatile shared_data_t *shared_data;
volatile u32 *virtual_7seg_pio;
static unsigned filter_px_count = 0;

//// se copio bien

char *output_file_decrypted;
char *output_file_filtered;

void *timer_thread();

void save_image(uint8_t *memory_start, const char *file_name) {
  FILE *pFile;
  pFile = fopen(file_name, "wb");

  if (pFile == NULL) {
    printf("Failed to open file.\n");
    return;
  }

  // Obtener tamaño de imagen con width*height
  size_t size = shared_data->image_h * shared_data->image_w;

  fwrite(memory_start, size, 1, pFile);

  fclose(pFile);
}

int main(int argc, char *argv[]) {
  seconds = time(NULL);
  printf("Todo esta bien\r\n");

  // rutas de archivo
  char *input_file = argv[1];
  output_file_decrypted = argv[2];
  output_file_filtered = argv[3];
  int nios_processing_percentage = atoi(argv[4]);
  int fd;
  (void)argc;
  //-------------------- Mapeado y Handshake --------------------------------//
  // Abrir /dev/mem
  if ((fd = open("/dev/mem", (O_RDWR | O_SYNC))) == -1) {
    printf("ERROR: could not open \"/dev/mem\"...\n");
    return (1);
  }
  // Mapear region del 7 segmentos
  void *map_result = mmap(NULL, HPS2FPGA_7SEG_SPAN, (PROT_READ | PROT_WRITE),
                          MAP_SHARED, fd, HPS2FPGA_7SEG_BASE);
  if (map_result == MAP_FAILED) {
    printf("Failed to map 7 segment address\r\n");
    close(fd);
    return 1;
  }

  virtual_7seg_pio = (u32 *)map_result;
  // Leer valor del puntero de los datos compartidos
  off_t shared_data_base = HPS2FPGA_BRIDGE_BASE + (*virtual_7seg_pio);
  printf("shared data lives at 0x%lx\r\n", (unsigned long)shared_data_base);
  // mapear datos compartidos en memoria virtual
  map_result = mmap(NULL, sizeof(shared_data_t), (PROT_READ | PROT_WRITE),
                    MAP_SHARED, fd, shared_data_base);
  if (map_result == MAP_FAILED) {
    printf("Failed to map shared data structure \r\n");
    close(fd);
    return 1;
  }
  shared_data = (shared_data_t *)map_result;

  shared_data->filter_hps_start = nios_processing_percentage;

  // realizar handshake para indicarle al Nios que ya se puede seguir
  *virtual_7seg_pio = HANDSHAKE_VAL_CALC(*virtual_7seg_pio);
  // Verificar mensaje
  printf("Embededd message: %s \r\n", shared_data->message);

  // copia contenidos del archivo a sdram
  FILE *file = fopen(input_file, "r");
  if (file == NULL) {
    perror("Error opening file");
    return 1;
  }

  int num;
  int result;
  int count = 0;
  while (1) {
    // Attempt to read an integer
    result = fscanf(file, "%d,", &num);
    // printf("num %d", num);
    if (result == EOF) {
      break; // End of file
    } else if (result == 1) {
      if (count == 0) {
        shared_data->image_w = (u32)num;
        count++;
        continue;
      }
      if (count == 1) {
        shared_data->image_h = (u32)num;
        count++;
        continue;
      }

      shared_data->image_encrypted[count - 2] = (u32)num;
      count++;
    } else {
      // Incomplete or invalid input, read and discard the rest of the line
      char buffer[1024];
      fgets(buffer, sizeof(buffer), file);
    }
  }

  fclose(file);

  // setea image_copy_done
  shared_data->image_copy_done = true;
  // spawn a proceso periodico que chequea si hay pixeles pa procesar
  printf("image copy done %d \n", shared_data->image_copy_done);
  pthread_t thread;
  if (pthread_create(&thread, NULL, timer_thread, NULL) != 0) {
    fprintf(stderr, "Error creando timer\n");
    return 1;
  }
  pthread_join(thread, NULL);
  // --------------------------- Final de programa ---------------------------//
  // eliminar mapeos
  if (munmap((void *)virtual_7seg_pio, HPS2FPGA_7SEG_SPAN) != 0) {
    printf("Failed to unmap 7 segment pio\r\n");
    close(fd);
    return 1;
  }
  if (munmap((void *)shared_data, sizeof(shared_data_t)) != 0) {
    printf("Failed to unmap shared data\r\n");
    close(fd);
    return 1;
  }

  return 0;
}

void proceso_periodico() {
  printf("proceso periodico %d\n", shared_data->decrypt_done);

  if (shared_data->decrypt_done && shared_data->nios_filter_done &&
      shared_data->hps_filter_done) {
    // compactar imagen
    uint8_t *compacted_decrypted = (uint8_t *)&shared_data->image_encrypted[0];
    for (size_t i = 0; i < (shared_data->image_w * shared_data->image_h); i++) {
      compacted_decrypted[i] = (uint8_t)shared_data->image_encrypted[i];
    }

    // guardar imagen descifrada
    save_image(compacted_decrypted, output_file_decrypted);
    printf("se guarda descifrada\n");

    // guardar imagen filtrada
    save_image((uint8_t *)&shared_data->image_filtered[0],
               output_file_filtered);
    printf("se guarda filtrada\n");

    // colocar valor de segundos en 7 segmentos
    time_t delta = time(NULL) - seconds;
    *virtual_7seg_pio = delta;
    printf("Procesamiento se tomo 0x%lx \n", delta);
    // Apagar thread
    pthread_exit(0);
  }

  // hay pixeles pa procesar por parte del HPS?
  //! if (shared_data->decrypt_px_count < shared_data->filter_hps_start) {
  if (!shared_data->decrypt_done) {
    return;
  }
  // si sí, procesa lo que hay

  // si hay más de dos filas descifradas, aplica filtro hasta agotar los pixeles
  // descifrados que se pueden filtrar
  int num_pixels_to_filter = 0;

  int unfiltered_decrypted_px_count = shared_data->decrypt_px_count -
                                      shared_data->filter_hps_start -
                                      filter_px_count;

  // Truncar a 0 (puede ser negativo si no ha llegado al filter_hps_start)
  if (unfiltered_decrypted_px_count < 0) {
    unfiltered_decrypted_px_count = 0;
  }

  // Esperar a que hayan 2 filas o esté desencriptado hasta el final
  if ((unsigned)unfiltered_decrypted_px_count > 5 * shared_data->image_w ||
      shared_data->decrypt_done) {
    num_pixels_to_filter = unfiltered_decrypted_px_count;
  }

  // Dar un buffer de dos filas que no toque hasta el final
  if (!shared_data->decrypt_done) {
    num_pixels_to_filter -= 2 * shared_data->image_w;
  }

  // Truncar a 0 (puede ser negativo)
  if (num_pixels_to_filter < 0) {
    num_pixels_to_filter = 0;
  }

  apply_filter((uint32_t *)&shared_data->image_encrypted[0],
               (uint8_t *)&shared_data->image_filtered[0],
               shared_data->filter_hps_start + filter_px_count,
               num_pixels_to_filter, shared_data->image_w,
               shared_data->image_h);
  filter_px_count += num_pixels_to_filter;

  printf("Cortex: pixeles filtrados %d\t%d%%\n", filter_px_count,
         100 * filter_px_count /
             (shared_data->image_w * shared_data->image_h -
              shared_data->filter_hps_start));

  // si ya llego a filter_hps_start deja de filtrar y apaga el
  // ISR. Setea nios_filter_done
  if (shared_data->filter_hps_start + filter_px_count ==
      shared_data->image_w * shared_data->image_h) {
    shared_data->hps_filter_done = true;
    printf("Cortex FINISH\n");
  }
}

void *timer_thread(void) {
  printf("se ejecuta \n");
  while (1) {
    sleep(0.001); // 1ms
    proceso_periodico();
    printf("se ejecuta \n");
  }
  printf("sale \n");
  return 0;
}
