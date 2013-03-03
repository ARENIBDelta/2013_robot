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
 * PE2 et PE3 simulent une sortie quadrature, pour tester
 * le bon fonctionnement du MSP
 *
 * Le fait de resetter les SPI des deux cartes est important,
 * car sinon il risque d'y avoir des décalages dans les
 * registres, et ainsi avoir des bugs aléatoires liés par
 * exemple à l'ordre dans lequel on a allumé les cartes.
 *
 * Pineout:
 * EK-LM4F120X  -> MSP-EXP430G2
 * PD0 (SSICLK) -> P1.4 (UCA0CLK)
 * PD2 (SSIRX)  -> P1.1 (UCA0SOMI)
 * PD3 (SSITX)  -> P1.2 (UCA0SIMO)
 * PE1 (PE1)    -> P1.5 (P1.5)
 * PE2 (PE2)    -> P2.0 (P2.0)
 * PE3 (PE3)    -> P2.1 (P2.1)
 */

unsigned short qeis[6];
unsigned short errs[6];

void spi_reset_init(void) {
	SysCtlPeripheralReset(SYSCTL_PERIPH_SSI1);
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
}

void main(void) {
	//Config sys
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    //Simulation quadrature
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_3);
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_2);

    //SPI CS
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_1);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, GPIO_PIN_1);

    unsigned int dt = 100000;

	while(1) {
		unsigned int ii;
		for (ii = 0; ii<10; ii++) {
			GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, 0);
			GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3, GPIO_PIN_3);
		    SysCtlDelay(dt);
			GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, GPIO_PIN_2);
			GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3, GPIO_PIN_3);
		    SysCtlDelay(dt);
			GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, GPIO_PIN_2);
			GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3, 0);
		    SysCtlDelay(dt);
			GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, 0);
			GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3, 0);
		    SysCtlDelay(dt);
		}

		unsigned char *p;
		unsigned long in;

		//SPI configure
		spi_reset_init();
		//SPI CS, High -> Low
		GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, 0);
		SysCtlDelay(1000);

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

		//SPI CS, Low => High
		GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, GPIO_PIN_1);

		//la patience est une vertue qui s'aquiert avec de la patience
		SysCtlDelay(1000000);
	}

}
