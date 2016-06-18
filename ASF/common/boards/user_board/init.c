/**
 * \file
 *
 * \brief User board initialization template
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>

#include "arduino_core/internal_timer.h"


#define DACC_ANALOG_CONTROL (DACC_ACR_IBCTLCH0(0x02) \
| DACC_ACR_IBCTLCH1(0x02) \
| DACC_ACR_IBCTLDACCORE(0x01))


void board_init(void)
{
	/* This function is meant to contain board-specific initialization code
	 * for, e.g., the I/O pins. The initialization can rely on application-
	 * specific board configuration, found in conf_board.h.
	 */
	WDT->WDT_MR = WDT_MR_WDDIS;  //disable watchdog timer
	
	//enable clocks to the PIO controllers so we can generate interrupts
	sysclk_enable_peripheral_clock(ID_PIOA);
	sysclk_enable_peripheral_clock(ID_PIOB);
	NVIC_EnableIRQ(PIOA_IRQn);
	NVIC_EnableIRQ(ID_PIOB);
	//pio_enable_interrupt(PIOA, 0xffffffff);
	//pio_enable_interrupt(PIOB, 0xffffffff);
	
	//init and start up the adc
	sysclk_enable_peripheral_clock(ID_ADC);
	adc_init(ADC, sysclk_get_main_hz(), sysclk_get_main_hz() / 4, 512);
	adc_configure_timing(ADC, 0, ADC_SETTLING_TIME_3, 1);
	adc_set_resolution(ADC, ADC_MR_LOWRES_BITS_12);
	//adc_enable_channel(ADC, ADC_CHANNEL_3);
	adc_start(ADC);

	//configure timer counter to create milliseconds counter for Arduino
	//delay/millis functions
	//internal_timer_configure(1000);
	start_microseconds_counter();
	
	//configure the DAC
	sysclk_enable_peripheral_clock(ID_DACC);
	dacc_reset(DACC);
	dacc_set_transfer_mode(DACC, 0); //half word mode?
	dacc_set_timing(DACC, 0x1, 0, 0xff);
	//dacc_enable_channel(DACC, 0); //enable channel 0
	//dacc_enable_channel(DACC, 1);
	dacc_set_analog_control(DACC, DACC_ANALOG_CONTROL);
	
	//start the usb stack regardless
	udc_start();
	
}
