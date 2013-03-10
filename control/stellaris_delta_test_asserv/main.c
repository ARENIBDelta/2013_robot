/*
 * Test d'asservissement d'un moteurs CC
 * 2013-03 Erwan Martin <erwan@martin.tl>
 *
 * Ce montage utilise un MSP-EXP430G2 pour récupérer la valeur des
 * codeurs, et un driver H pour commander le moteur.
 * Ne pas oublier de mettre des résistances de tirage de 10k environ
 * pour charger les sorties codeurs du moteur.
 *
 * Pineout SPI:
 * EK-LM4F120X  -> MSP-EXP430G2
 * PD0 (SSICLK) -> P1.4 (UCA0CLK)
 * PD2 (SSIRX)  -> P1.1 (UCA0SOMI)
 * PD3 (SSITX)  -> P1.2 (UCA0SIMO)
 * PE1 (PE1)    -> P1.5 (P1.5)
 * PE2 (PE2)    -> P2.0 (P2.0)
 * PE3 (PE3)    -> P2.1 (P2.1)
 *
 * Pineout H:
 * VCC          -> VCC Codeurs
 * PB4          -> PWM Moteur 1
 * PE2          -> Dir Moteur 1
 */

#define PART_LM4F120B2QR
#include <inc/hw_types.h>
#include "driverlib/fpu.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/pin_map.h"
#include "inc/hw_ints.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include <stdint.h>
#include <stdlib.h>

#include "parts/control.h"

void main(void) {
	//Config sys
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);
    control_init();

	while(1) {
		//la patience est une vertue qui s'aquiert avec de la patience
		SysCtlDelay(1000);
	}

}
