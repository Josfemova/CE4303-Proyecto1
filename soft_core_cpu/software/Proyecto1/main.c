
#include <stdio.h>
#include "system.h"
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_timer_regs.h"
#include "types.h"
#include <math.h>
#include "sys/alt_irq.h"

void timer_isr(){
  // si hay más de dos filas descifradas, aplica filtro hasta agotar los pixeles
  // descifrados que se pueden filtrar 

  // si ya llego a filter_hps_start deja de filtrar y apaga el ISR. Setea nios_filter_done
}



int d = 0;
int n = 0;
u32* bsdram = (u32*) SDRAM_BASE;
u32* esdram = (u32*)(SDRAM_BASE + SDRAM_SPAN);

#define START_MASK 0x1
#define STEP_MASK 0x2
#define SHIFT_MASK 0x4
#define UP_MASK 0x8
#define ACTION_BTN_MASK (0x1 | 0x2 | 0x4 | 0x8)
#define ACTION_SW_MASK 0x1

#define MANUAL_MODE 0
#define AUTOMATIC_MODE 1

#define LAST_7SEG 0x5

unsigned static current_7_seg = 0;
unsigned static current_pixel = 0;
unsigned static has_started = 0;


unsigned get_mode()
{
	unsigned pio_sw = IORD_ALTERA_AVALON_PIO_DATA(PIO_SW_BASE);
	return (pio_sw & 0x2) >> 1;

}

void increase_digit(int* number, int position){
    int digit = (*number >> (position * 4)) & 0xF;
    int newDigit = digit + 2;

    // Calculate a mask to clear the old digit at the specified position
    unsigned int clearMask = ~(0xF << (position * 4));

    // Clear the old digit
    *number &= clearMask;

    // Shift and add the new digit to the number
    *number |= (newDigit & 0xF) << (position * 4);

    IOWR_ALTERA_AVALON_PIO_DATA(PIO_7SEG_BASE, *number);
 }


void sw_isr(void* context){

	int key_selected = IORD_ALTERA_AVALON_PIO_DATA(PIO_SW_BASE) & 0x1;
	if(key_selected){
		IOWR_ALTERA_AVALON_PIO_DATA(PIO_7SEG_BASE, d);
	} else {
		IOWR_ALTERA_AVALON_PIO_DATA(PIO_7SEG_BASE, n);
	}
}

void btn_isr(void* context){
	int edge_capture = IORD_ALTERA_AVALON_PIO_EDGE_CAP(PIO_BTN_BASE);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_BTN_BASE, 0x0); // limpiar el isr
	switch(edge_capture & ACTION_BTN_MASK){
	case UP_MASK:
		if(IORD_ALTERA_AVALON_PIO_DATA(PIO_SW_BASE) & 0x1){
			increase_digit(&d, current_7_seg);
		} else {
			increase_digit(&n, current_7_seg);
		}
		break;
	case SHIFT_MASK:
		current_7_seg = current_7_seg == LAST_7SEG ? 0 : (current_7_seg + 1);
		break;
	case STEP_MASK:
		if(get_mode() == MANUAL_MODE){
			current_pixel += 1;
		}
		break;
	case START_MASK:
		has_started = 1;
		break;
	default:
		break;
	}
	// retardo para consideraciones de latencia, ver:
	// https://www.intel.com/content/www/us/en/docs/programmable/683525/21-3/an-isr-to-service-a-button-pio-interrupt.html
	IORD_ALTERA_AVALON_PIO_EDGE_CAP(PIO_BTN_BASE);
}


int main()
{

	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(PIO_BTN_BASE, ACTION_BTN_MASK);
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(PIO_SW_BASE, ACTION_SW_MASK);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_BTN_BASE, 0x0);

	alt_ic_isr_register(
		PIO_BTN_IRQ_INTERRUPT_CONTROLLER_ID,
		PIO_BTN_IRQ,
		btn_isr,
		NULL,
		NULL
	);

	alt_ic_isr_register(
		PIO_SW_IRQ_INTERRUPT_CONTROLLER_ID,
		PIO_SW_IRQ,
		sw_isr,
		NULL,
		NULL
	);


	while(has_started != 1); // TODO: Add condition image_copy_done = 1;

	int decrypt_px_count = 0; // TODO: Read value from header
	int width = 20; // TODO: Read value from header
	int height = 40; // TODO: Read value from header
	while(decrypt_px_count != (width * height))
	{
		unsigned mode = get_mode();

		if(mode == MANUAL_MODE){
			continue;
			// Esperar por la interrupcion del boton
		}

		if(mode == AUTOMATIC_MODE){
			// TODO:  Decrypt pixel
		}
		IOWR_ALTERA_AVALON_PIO_DATA(PIO_7SEG_BASE, current_pixel);
	}

	// TODO: Set decrypt done




	while(1);
	return 0;
}
