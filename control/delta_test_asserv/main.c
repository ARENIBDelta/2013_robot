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
#include "platform/pwm.h"
#include "platform/uartbt.h"
#include "tools/delta_calc.h"

#include "math.h"
#define M_PI 3.14159265358979323846
extern unsigned short next_goals[6];
extern unsigned char control_reached;

void main(void) {
	//Config sys
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);

    config_pwms();
    control_init();

    //Init quadrature
    control_go_to_origin();
	while(!control_reached) {
		SysCtlDelay(1000);
	}
	control_start();

	float t = 0;
	unsigned short alpha_a, beta_a, gamma_a;
	while(1) {
		delta_params params;
		params.a = 70,
		params.b = 225;
		params.c = 50;
		params.d = 50;
		float alpha, beta, gamma;
		float X, Y, Z;
		t += 2 * M_PI / 50;
		X = 30*cos(t);
		Y = 30*sin(t);
		Z = 220;

		//Z = 220 + 20 * cos(t);

		//Ne pas oublier de vérifier la taille de la stack avant d'utiliser ça
		delta_calc(params, X, Y, Z, 0, &alpha, &beta, &gamma);
		alpha_a = 10245 - ((alpha / M_PI * 180) - 180 )/0.108;
		beta_a = 10245 - ((beta / M_PI * 180) - 180 )/0.108;
		gamma_a = 10245 - ((gamma / M_PI * 180) - 180 )/0.108;

		alpha_a = alpha_a > 10700 ? 10700 : alpha_a;
		alpha_a = alpha_a < 9800 ? 9800 : alpha_a;
		beta_a = beta_a > 10700 ? 10700 : beta_a;
		beta_a = beta_a < 9800 ? 9800 : beta_a;
		gamma_a = gamma_a > 10700 ? 10700 : gamma_a;
		gamma_a = gamma_a < 9800 ? 9800 : gamma_a;

		while(!control_reached) {
			SysCtlDelay(1000);
		}

		control_set_goal(alpha_a, beta_a, gamma_a);
		control_start();

		//la patience est une vertue qui s'aquiert avec de la patience
		SysCtlDelay(1000);
	}

}
