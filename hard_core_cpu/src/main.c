#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>

#include "misc.h"
#include "types.h"

int main(int argc, char* argv[]) {
  // rutas de archivo
  char* input_file = argv[1];
  char* output_file_decrypted = argv[2];
  char* output_file_filtered = argv[3];
  int nios_processing_percentage = atoi(argv[4]);
  int fd;
  (void)argc;
  (void)output_file_decrypted;
  (void)output_file_filtered;
  (void)nios_processing_percentage;
  //-------------------- Mapeado y Handshake --------------------------------//
  // Abrir /dev/mem
  if ((fd = open("/dev/mem", (O_RDWR | O_SYNC))) == -1) {
    printf("ERROR: could not open \"/dev/mem\"...\n");
    return (1);
  }
  // Mapear region del 7 segmentos
  void* map_result = mmap(NULL, HPS2FPGA_7SEG_SPAN, (PROT_READ | PROT_WRITE),
                          MAP_SHARED, fd, HPS2FPGA_7SEG_BASE);
  if (map_result == MAP_FAILED) {
    printf("Failed to map 7 segment address\r\n");
    close(fd);
    return 1;
  }

  volatile u32* virtual_7seg_pio = (u32*)map_result;
  // Leer valor del puntero de los datos compartidos
  off_t shared_data_base = HPS2FPGA_BRIDGE_BASE + (*virtual_7seg_pio);
  printf("shared data lives at 0x%lx\r\n", shared_data_base);
  // mapear datos compartidos en memoria virtual
  map_result = mmap(NULL, sizeof(shared_data_t), (PROT_READ | PROT_WRITE),
                    MAP_SHARED, fd, shared_data_base);
  if (map_result == MAP_FAILED) {
    printf("Failed to map shared data structure \r\n");
    close(fd);
    return 1;
  }
  volatile shared_data_t* shared_data = (shared_data_t*)map_result;

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

        if (result == EOF) {
            break; // End of file
        } else if (result == 1) {

            if(count == 0){
              shared_data->image_w = num;
              continue;
            }
            if(count == 1){
              shared_data->image_h = num;
              continue;
            }

            shared_data->image_encrypted[count - 2] = num;

        } else {
            // Incomplete or invalid input, read and discard the rest of the line
            char buffer[1024];
            fgets(buffer, sizeof(buffer), file);
        }
    }

    fclose(file);
    return 0;

  // setea image_copy_done
  shared_data->image_copy_done = 1;
  // spawn a proceso periodico que chequea si hay pixeles pa procesar

  // --------------------------- Final de programa ---------------------------//
  // eliminar mapeos
  if (munmap((void*)virtual_7seg_pio, HPS2FPGA_7SEG_SPAN) != 0) {
    printf("Failed to unmap 7 segment pio\r\n");
    close(fd);
    return 1;
  }
  if (munmap((void*)shared_data, sizeof(shared_data_t)) != 0) {
    printf("Failed to unmap shared data\r\n");
    close(fd);
    return 1;
  }

  return 0;
}

void* proceso_periodico() {
  // fijarse en header de programa de sdram

  // hay pixeles pa procesar por parte del HPS?
  // si sí, procesa lo que hay

  // si no, si no ha terminado hace yield

  // si ya termino, guarda ambas imagenes a sd
  return 0;
}
