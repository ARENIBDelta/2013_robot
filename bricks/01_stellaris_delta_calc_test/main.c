/*
 * Tests des fonctions de calculs de bras delta.
 * Erwan Martin <erwan@martin.tl>
 * ARENIB Delta
 * License: MIT
 *
 * Ce programme calcule en boucle des valeurs d'angles pour les bras
 * du robot Delta. Ceci permet de mesurer le temp d'exécution de
 * cette fonction.
 *
 * Attention à la taille de la pile si vous utilisez cette fonction.
 * La valeur par défaut peut être trop faible selon votre chaîne de
 * développement.
 *
 * Developpé et testé sur EK-LM4F120XL (LM4F120H5QR).
 *
 * */

#include "driverlib/fpu.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"

#include "delta_calc.h"

void main(void) {
	FPUEnable();
	FPULazyStackingEnable();
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	while(1) {
		delta_params params;
		params.a = 70,
		params.b = 200;
		params.c = 50;
		params.d = 70;
		float alpha, beta, gamma;
		float X, Y, Z;
		X = 20;
		Y = -40;
		Z = 186;
		delta_calc(params, X, Y, Z, 0, &alpha, &beta, &gamma);
	}
}
