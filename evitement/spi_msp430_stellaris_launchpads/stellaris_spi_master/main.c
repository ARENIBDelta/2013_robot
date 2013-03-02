#define PART_LM4F120B2QR
#include <inc/hw_types.h>
#include "driverlib/fpu.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/pin_map.h"
#include "inc/hw_ssi.h"
#include "driverlib/ssi.h"
#include "inc/hw_ints.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"

#include "layer2p.h"
#include "stdlib.h"

int truc;
layer2p_ctx ctx;

void spi_transmit(unsigned char data, unsigned char lastbyte) {
	SSIDataPut(SSI1_BASE, data);
}

/*
 * Test de communication SPI entre un kit EK-LM4F120XL et un kit
 * MSP-EXP430G2.
 * 2013-02-14 Erwan Martin <erwan@martin.tl>
 *
 * Pineout:
 * EK-LM4F120X  -> MSP-EXP430G2
 * PD0 (SSICLK) -> P1.4 (UCA0CLK)
 * PD2 (SSIRX)  -> P1.1 (UCA0SOMI)
 * PD3 (SSITX)  -> P1.2 (UCA0SIMO)
 */

void main(void) {

    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);

    //Configuration SPI
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    GPIOPinConfigure(GPIO_PD0_SSI1CLK);
    GPIOPinConfigure(GPIO_PD1_SSI1FSS);
    GPIOPinConfigure(GPIO_PD2_SSI1RX);
    GPIOPinConfigure(GPIO_PD3_SSI1TX);
    GPIOPinTypeSSI(GPIO_PORTD_BASE,
    		GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    SSIConfigSetExpClk(SSI1_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
                       SSI_MODE_MASTER, 10000, 8);
    SSIEnable(SSI1_BASE);

	layer2p_ctx_init(&ctx);
	layer2p_set_linksend_callback(&ctx, &spi_transmit);
	layer2p_reset(&ctx);

	struct some_data {
		char chars[5];
		unsigned int a1;
		unsigned int a2;
		unsigned int a3;
	} data;

	data.chars[0] = 'A';
	data.chars[1] = 0xAA;
	data.chars[2] = 'C';
	data.chars[3] = 'D';
	data.chars[4] = 'E';
	data.a1 = 0x11111111;
	data.a2 = 0x22222222;
	data.a3 = 0x33333333;

	while(1) {
		layer2p_send(&ctx, (unsigned char *)&data, sizeof(data));
		SysCtlDelay(1000000);
	}

}
