
#include <math.h>
#include <stdio.h>

#include "altera_avalon_pio_regs.h"
#include "altera_avalon_timer_regs.h"
#include "sys/alt_irq.h"
#include "system.h"
#include "types.h"
#include <string.h>
#include <stdbool.h>

// Alineado a 4KB porque el mapeo lo requiere
volatile shared_data_t shared_data __attribute__((aligned(0x1000))) = {};
int d = 0;
int n = 0;
int width;
int height;

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
unsigned static decrypt_px_count = 0;
unsigned static has_started = 0;

unsigned get_mode() {
  unsigned pio_sw = IORD_ALTERA_AVALON_PIO_DATA(PIO_SW_BASE);
  return (pio_sw & 0x2) >> 1;
}

void start(){
	  has_started = 1;
	  IOWR_ALTERA_AVALON_PIO_DATA(PIO_7SEG_BASE, 0x0); // Set 7seg to zero

	  IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER1_100US_BASE,
			  	  ALTERA_AVALON_TIMER_CONTROL_ITO_MSK
				| ALTERA_AVALON_TIMER_CONTROL_CONT_MSK
				| ALTERA_AVALON_TIMER_CONTROL_START_MSK);

}

bool isImageCopyDone(){
	bool imageCopyDone = shared_data.image_copy_done;
	return imageCopyDone;
}

// Right-to-left binary method
// https://en.wikipedia.org/wiki/Modular_exponentiation#Right-to-left_binary_method
u32 mod_exp(u32 base, u32 exponent, u32 modulus) {
    u32 result = 1;
    base %= modulus;

    while (exponent > 0) {
        if (exponent % 2 == 1) {
            result = (result * base) % modulus;
        }

        base = (base * base) % modulus;
        exponent /= 2; //assuming this works as a floor function
    }

    return result;
}

void decrypt_px(){
	u32 current_pixel = shared_data.image_encrypted[decrypt_px_count];
	
  current_pixel = mod_exp(current_pixel, d, n)

	IOWR_ALTERA_AVALON_PIO_DATA(PIO_7SEG_BASE, current_pixel);
	decrypt_px_count += 1;
}

void timer1_100us_isr(void* context){

	if(decrypt_px_count != (width * height) && isImageCopyDone()) {
	    unsigned mode = get_mode();
	    if (mode == AUTOMATIC_MODE) {
	        decrypt_px();
	    }
	  }

	// Limpiar el isr
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER1_100US_BASE, 0);
}

void timer0_1ms_isr(void* context) {
  // si hay más de dos filas descifradas, aplica filtro hasta agotar los pixeles
  // descifrados que se pueden filtrar

  // si ya llego a filter_hps_start deja de filtrar y apaga el ISR. Setea
  // nios_filter_done
}

void increase_digit(int* number, int position) {
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

void sw_isr(void* context) {
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_SW_BASE, 0x0);  // limpiar el isr
	int key_selected = IORD_ALTERA_AVALON_PIO_DATA(PIO_SW_BASE) & 0x1;
	if (key_selected) {
		IOWR_ALTERA_AVALON_PIO_DATA(PIO_7SEG_BASE, d);
	} else {
		IOWR_ALTERA_AVALON_PIO_DATA(PIO_7SEG_BASE, n);
	}
}

void increase(){
    if (IORD_ALTERA_AVALON_PIO_DATA(PIO_SW_BASE) & 0x1) {
      increase_digit(&d, current_7_seg);
    } else {
      increase_digit(&n, current_7_seg);
    }

}

void btn_isr(void* context) {
  int edge_capture = IORD_ALTERA_AVALON_PIO_EDGE_CAP(PIO_BTN_BASE);
  IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_BTN_BASE, 0x0);  // limpiar el isr
  switch (edge_capture & ACTION_BTN_MASK) {
    case UP_MASK:
    	if(has_started == 0){
    		increase();
    	}
      break;
    case SHIFT_MASK:
    	if(has_started == 0){
    		current_7_seg = current_7_seg == LAST_7SEG ? 0 : (current_7_seg + 1);
    	}
      break;
    case STEP_MASK:
    	if(has_started == 1 && isImageCopyDone()){
    		if (get_mode() == MANUAL_MODE) {
    			decrypt_px();
    		}
    	}
      break;
    case START_MASK:
    	start();
      break;
    default:
      break;
  }
  // retardo para consideraciones de latencia, ver:
  // https://www.intel.com/content/www/us/en/docs/programmable/683525/21-3/an-isr-to-service-a-button-pio-interrupt.html
  IORD_ALTERA_AVALON_PIO_EDGE_CAP(PIO_BTN_BASE);
}

int main() {
  printf("Hola\r\n");
  strcpy((char*)shared_data.message, "System OK");
  IOWR_ALTERA_AVALON_PIO_DATA(PIO_7SEG_BASE, 0xFAFAFA);
  // Initial handshake
  uintptr_t shared_data_offset = ((uintptr_t)&shared_data) & 0x00FFFFFF;
  printf("datos en 0x%x\r\n", shared_data_offset);
  uintptr_t hps_handshake_val = HANDSHAKE_VAL_CALC(shared_data_offset);
  // El puntero a datos compartidos se pasa via el reg del 7 Segmentos
  IOWR_ALTERA_AVALON_PIO_DATA(PIO_7SEG_BASE, shared_data_offset);
  while (IORD_ALTERA_AVALON_PIO_DATA(PIO_7SEG_BASE) != hps_handshake_val) {
    // esperar a que el HPS confirme que recibió la dirección para el handshake
  }
  printf("Inicio\r\n");
  IOWR_ALTERA_AVALON_PIO_DATA(PIO_7SEG_BASE, 0x0);

  IOWR_ALTERA_AVALON_PIO_IRQ_MASK(PIO_BTN_BASE, ACTION_BTN_MASK);
  IOWR_ALTERA_AVALON_PIO_IRQ_MASK(PIO_SW_BASE, ACTION_SW_MASK);
  IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_BTN_BASE, 0x0);
  IOWR_ALTERA_AVALON_PIO_EDGE_CAP(PIO_SW_BASE, 0x0);

  height = shared_data.image_h;
  width = shared_data.image_w;

  alt_ic_isr_register(PIO_BTN_IRQ_INTERRUPT_CONTROLLER_ID, PIO_BTN_IRQ, btn_isr,
                      NULL, NULL);

  alt_ic_isr_register(PIO_SW_IRQ_INTERRUPT_CONTROLLER_ID, PIO_SW_IRQ, sw_isr,
                      NULL, NULL);

  alt_ic_isr_register(TIMER1_100US_IRQ_INTERRUPT_CONTROLLER_ID, TIMER1_100US_IRQ,timer1_100us_isr,
		  	  	  	  NULL,
					  NULL
  );

  while (1);
  return 0;
}
