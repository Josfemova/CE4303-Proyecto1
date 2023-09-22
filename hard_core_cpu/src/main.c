#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#include "misc.h"
#include "types.h"

int main(int argc, char* argv[]) {
  // rutas de archivo
  char* input_file = argv[1];
  char* output_file_decrypted = argv[2];
  char* output_file_filtered = argv[3];
  int fd;
  (void)argc;
  (void)input_file;
  (void)output_file_decrypted;
  (void)output_file_filtered;
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
  shared_data->image_copy_done = false;

  // abrir archivo
  // copia contenidos del archivo a sdram
  // setea image_copy_done
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
