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

#include "stdlib.h"

/*
 * Communication SPI entre un kit EK-LM4F120XL et un kit
 * MSP-EXP430G2.
 * 2013-02-14 Erwan Martin <erwan@martin.tl>
 *
 * Deux commandes distinctes permettent de récupérer soit
 * 12 octets (quadratures) soit 12 octets (erreurs).
 *
 * Pineout:
 * EK-LM4F120X  -> MSP-EXP430G2
 * PD0 (SSICLK) -> P1.4 (UCA0CLK)
 * PD2 (SSIRX)  -> P1.1 (UCA0SOMI)
 * PD3 (SSITX)  -> P1.2 (UCA0SIMO)
 */

unsigned short qeis[6];
unsigned short errs[6];

void main(void) {
	//Config sys
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
    SSIConfigSetExpClk(SSI1_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_3,
                       SSI_MODE_MASTER, 1000, 8);
    SSIEnable(SSI1_BASE);

	while(1) {
		unsigned char *p;
		unsigned long in;

		//récupération des quadratures
		p = (unsigned char *)qeis;
		SSIDataPut(SSI1_BASE, 0x55);
		SSIDataGet(SSI1_BASE, &in);
		SysCtlDelay(1000);
		unsigned char i;
		for (i=12;i--;) {
			SSIDataPut(SSI1_BASE, 0x00);
			SSIDataGet(SSI1_BASE, &in);
			*(p++) = in & 0xFF;
			SysCtlDelay(1000);
		}

		//récupération des erreurs
		p = (unsigned char *)errs;
		SSIDataPut(SSI1_BASE, 0xAA);
		SSIDataGet(SSI1_BASE, &in);
		SysCtlDelay(1000);
		for (i=12;i--;) {
			SSIDataPut(SSI1_BASE, 0x00);
			SSIDataGet(SSI1_BASE, &in);
			*(p++) = in & 0xFF;
			SysCtlDelay(1000);
		}

		//la patience est une vertue qui s'aquiert avec de la patience
		SysCtlDelay(1000000);
	}

}
