#include "driverlib/fpu.h"
#include "driverlib/rom.h"
#include "inc/lm4f120h5qr.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"

/*
 * Ce fichier a pour but de tester les fonctions de calcul du bras delta
 * sur la cible.
 */

#include "delta_calc.h"

void main(void) {
	FPUEnable();
	FPULazyStackingEnable();
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOF;
    GPIO_PORTF_DIR_R = 0x08;
    GPIO_PORTF_DEN_R = 0x08;
	while(1) {
		delta_params params;
		params.a = 70,
		params.b = 100;
		params.c = 130;
		params.d = 140;
		float alpha, beta, gamma;
		float X, Y, Z;
		X = 0;
		Y = 0;
		Z = 150;
		GPIO_PORTF_DATA_R ^= 0x08;
		delta_calc(params, X, Y, Z, 0, &alpha, &beta, &gamma);
	}
}
