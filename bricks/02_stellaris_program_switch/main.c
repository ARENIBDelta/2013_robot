#define PART_LM4F120H5QR

/* Démonstration du changement de contexte du mode utilisateur par une interruption.
 * Erwan Martin <erwan@martin.tl>
 * ARENIB Delta
 * License: MIT
 *
 * Ce programme exécute son main(), et est interrompu au bout de 5s par un timer
 * qui va alors rediriger le flux sur son autre main(). Ce dernier va alors
 * retrouver un environnement propre en réinitialisant la pile.
 *
 * Developpé et testé sur EK-LM4F120XL (LM4F120H5QR).
 *
 * */


#include <stdint.h>
#include <inc/hw_types.h>
#include <inc/hw_memmap.h>
#include <inc/hw_ints.h>
#include <inc/hw_timer.h>
#include <driverlib/sysctl.h>
#include <driverlib/timer.h>
#include <driverlib/pin_map.h>
#include <driverlib/interrupt.h>

extern unsigned long __STACK_END;
void reset_stack(unsigned long stack_addr);
void edit_user_pc(unsigned int address);

int main2(void) {
	//Réinitialisation de la pile
	reset_stack((unsigned int) &__STACK_END);

	//Application 2
	while(1);
}

int main(void) {
	//Le timer w3 est configuré pour générer une interruption toute les 5 secondes
	SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER3);
	TimerConfigure(WTIMER3_BASE, TIMER_CFG_PERIODIC);
	TimerLoadSet64(WTIMER3_BASE, SysCtlClockGet()*5); //5 secondes
	IntEnable(INT_WTIMER3A);
	TimerIntEnable(WTIMER3_BASE, TIMER_TIMA_TIMEOUT);

	//Début du comptage
	TimerEnable(WTIMER3_BASE, TIMER_A);

	//Application 1
	while(1);
}

void WTimer3IntHandler(void) {
	//Altération du compteur programme pour changer de programme
	edit_user_pc((unsigned int)&main2);

	//Désactivation du timer
	TimerDisable(WTIMER3_BASE, TIMER_A);

	//Vide le drapeau d'interruption
	TimerIntClear(WTIMER3_BASE, TIMER_TIMA_TIMEOUT);
}



