
#include <stdio.h>
#include "system.h"
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_timer_regs.h"
#include <inttypes.h>

void timer_isr(){
  // si hay más de dos filas descifradas, aplica filtro hasta agotar los pixeles
  // descifrados que se pueden filtrar 

  // si ya llego a filter_hps_start deja de filtrar y apaga el ISR. Setea nios_filter_done
}

uint16_t d;
uint16_t n;


int main()
{
  IOWR_ALTERA_AVALON_PIO_DATA(PIO_7SEG_BASE,0xFAFAFA);
  printf("Hello from Nios II!\n");

  // espera a que image_copy_done == true
  
  // leer botones y switches para ingresar las claves de descifrado 

  // una vez listas las claves entra en otro modo que es descifrado 

  // configurar isr para proceso de filtro

  // loop
    // chequea switch de modo de descifrado 
    // si es manual, descifra hasta ver boton
    // si es automático descifra de un solo, termina loop y pasa a siguiente
    // pixel 
    // condición de salida: decrypt_px_count = width * height 
  
  // setear decrypt done
  return 0;
}
