
#include <stdio.h>
#include "system.h"
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_timer_regs.h"
#include <inttypes.h>


int main()
{
  IOWR_ALTERA_AVALON_PIO_DATA(PIO_7SEG_BASE,0xFAFAFA);
  printf("Hello from Nios II!\n");

  return 0;
}
