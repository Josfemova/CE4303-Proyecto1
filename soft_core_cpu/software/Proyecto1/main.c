
#include <stdio.h>
#include "system.h"
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_timer_regs.h"
#include "types.h"

void timer_isr(){
  // si hay más de dos filas descifradas, aplica filtro hasta agotar los pixeles
  // descifrados que se pueden filtrar 

  // si ya llego a filter_hps_start deja de filtrar y apaga el ISR. Setea nios_filter_done
}



u16 d;
u16 n;
u32* bsdram = (u32*) SDRAM_BASE;
u32* esdram = (u32*)(SDRAM_BASE + SDRAM_SPAN);


int main()
{
  IOWR_ALTERA_AVALON_PIO_DATA(PIO_7SEG_BASE,0xFAFA);
  //printf("Hemlo");
  while (IORD_ALTERA_AVALON_PIO_DATA(PIO_7SEG_BASE)!= 0xfefe);
  int x = 0x200;
  printf("HPS escribio a los leds 0x%x!\r\n", x);
  volatile u32* current  = bsdram;
  int val= 0;
  while (current != (bsdram + x)){
	  *current = val++;
	  current++;
	  printf("escribe val 0x%x!\r\n", val);
  }
  IOWR_ALTERA_AVALON_PIO_DATA(PIO_7SEG_BASE,0xF1F1);

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
  while(1);
  return 0;
}
