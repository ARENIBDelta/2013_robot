#define PART_LM4F120B2QR
#include <stdint.h>
#include <inc/hw_types.h>
#include <inc/hw_memmap.h>
#include <inc/hw_gpio.h>
#include <inc/hw_ints.h>
#include <inc/hw_timer.h>
#include <driverlib/sysctl.h>
#include <driverlib/timer.h>
#include <driverlib/pin_map.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/interrupt.h>

#include "actuators.h"

extern edit_user_pc(unsigned int address);
void main_match_ended(void);

static unsigned char state = 0;

void match_timer_init(void) {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER3);
	TimerConfigure(WTIMER3_BASE, TIMER_CFG_PERIODIC);
	//TimerLoadSet64(WTIMER3_BASE, 0x10C388D00); //90s
	TimerLoadSet64(WTIMER3_BASE, SysCtlClockGet()*5);

	IntEnable(INT_WTIMER3A);
	TimerIntEnable(WTIMER3_BASE, TIMER_TIMA_TIMEOUT);

//	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
//	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
//	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
}

void WTimer3IntHandler(void) {
	switch (state) {
	case 2:
		actuator_pwm_zero();
		TimerDisable(WTIMER3_BASE, TIMER_A);
		break;
	case 1:
		actuator_pwm_30percent();
		TimerLoadSet64(WTIMER3_BASE, SysCtlClockGet()*5);
		state = 2;
		break;
	case 0:
	default:
		state = 1;
		actuator_pwm_50percent();
		TimerLoadSet64(WTIMER3_BASE, SysCtlClockGet()*2);
		//GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
		edit_user_pc((unsigned int)&main_match_ended);
		break;
	}
	TimerIntClear(WTIMER3_BASE, TIMER_TIMA_TIMEOUT);
}

void match_timer_start(void) {
	TimerEnable(WTIMER3_BASE, TIMER_A);
}

