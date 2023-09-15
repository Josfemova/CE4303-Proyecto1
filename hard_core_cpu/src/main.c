#include <stdio.h>
#include <stdbool.h>

/* Como luce la sdram
0x0000_0000 : Header de programa
0x0000_0100 : Datos de imagen original
0x0030_0100 : Datos de imagen filtrada 
*/

struct shared_header{
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

int main(){
    printf("Hello World");
    // abrir archivo 
    // mapear region de sdram de la fpga con mmap
    // copia contenidos del archivo a sdram
    // setea image_copy_done
    // spawn a proceso periodico que chequea si hay pixeles pa procesar 
    return 0;
}



void* proceso_periódico(){
    // fijarse en header de programa de sdram

    // hay pixeles pa procesar por parte del HPS?
    // si sí, procesa lo que hay

    // si no, si no ha terminado hace yield

    // si ya termino, guarda ambas imagenes a sd
    return 0;
}



