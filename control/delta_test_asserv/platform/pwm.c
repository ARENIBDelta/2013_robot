#define PART_LM4F120B2QR
#include "inc/hw_timer.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include <stdint.h>

#include "pwm.h"

void set_pwm_width(uint32_t timer_base, uint32_t timer, uint32_t pulse_duration, uint8_t pulse_unit) {
	   uint32_t pwm_period;
	   switch(pulse_unit) {
		   case PULSE_100NS:
			   pwm_period = tics_100ns*pulse_duration;
			   break;
		   case PULSE_MS:
			   pwm_period = tics_ms*pulse_duration;
			   break;
		   case PULSE_US:
		   default:
			   pwm_period = tics_us*pulse_duration;
			   break;
	   }

	   uint8_t pwm_extender = pwm_period >> 16;
	   pwm_period &= 0xFFFF;
	   //uint8_t pwm_extender = 0;

	   TimerPrescaleMatchSet(timer_base, timer, pwm_extender);
       TimerMatchSet(timer_base, timer, pwm_period);
}

void config_pwms(void) {
	uint32_t frequency = 50000;

	uint32_t period1 = SysCtlClockGet() / frequency;
	uint16_t extender1 = 0;
	period1 &= 0xFFFF;

	uint32_t period2 =  SysCtlClockGet() / frequency;
	uint16_t extender2 = 0;
	period2 &= 0xFFFF;

	//WTimer 0.B => PC5
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIOPinConfigure(GPIO_PC5_WT0CCP1);
	GPIOPinTypeTimer(GPIO_PORTC_BASE, GPIO_PIN_5);
	// Configure timer
	SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0);
	TimerConfigure(WTIMER0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_PWM);
	//set period
	TimerPrescaleSet(WTIMER0_BASE, TIMER_B, extender1);
	TimerLoadSet(WTIMER0_BASE, TIMER_B, period1);
	//set default value B
	TimerPrescaleMatchSet(WTIMER0_BASE, TIMER_B, extender2);
	TimerMatchSet(WTIMER0_BASE, TIMER_B, period2);
	TimerEnable(WTIMER0_BASE, TIMER_B);
	set_pwm_width(WTIMER0_BASE, TIMER_B, 50, PULSE_US);

	//WTimer 1.AB => PC6 PC7
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIOPinConfigure(GPIO_PC6_WT1CCP0);
	GPIOPinConfigure(GPIO_PC7_WT1CCP1);
	GPIOPinTypeTimer(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);
	// Configure timer
	SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER1);
	TimerConfigure(WTIMER1_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM | TIMER_CFG_B_PWM);
	//set period
	TimerPrescaleSet(WTIMER1_BASE, TIMER_A, extender1);
	TimerLoadSet(WTIMER1_BASE, TIMER_A, period1);
	TimerPrescaleSet(WTIMER1_BASE, TIMER_B, extender1);
	TimerLoadSet(WTIMER1_BASE, TIMER_B, period1);
	//set default value A
	TimerPrescaleMatchSet(WTIMER1_BASE, TIMER_A, extender2);
	TimerMatchSet(WTIMER1_BASE, TIMER_A, period2);
	//set default value B
	TimerPrescaleMatchSet(WTIMER1_BASE, TIMER_B, extender2);
	TimerMatchSet(WTIMER1_BASE, TIMER_B, period2);
	TimerEnable(WTIMER1_BASE, TIMER_A);
	TimerEnable(WTIMER1_BASE, TIMER_B);
	set_pwm_width(WTIMER1_BASE, TIMER_A, 50, PULSE_US);
	set_pwm_width(WTIMER1_BASE, TIMER_B, 50, PULSE_US);

	//WTimer 5.AB => PD6 PD7
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	GPIOPinConfigure(GPIO_PD6_WT5CCP0);
	GPIOPinConfigure(GPIO_PD7_WT5CCP1);
    //
	// Disable NMI on PD7
    // Enable port PD7 for PWM
    // First open the lock and select the bits we want to modify in the GPIO commit register.
    //
    HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY_DD;
    HWREG(GPIO_PORTD_BASE + GPIO_O_CR) = 0x80;
	GPIOPinTypeTimer(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);
	// Configure timer
	SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER5);
	TimerConfigure(WTIMER5_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM | TIMER_CFG_B_PWM);
	//set period
	TimerPrescaleSet(WTIMER5_BASE, TIMER_A, extender1);
	TimerLoadSet(WTIMER5_BASE, TIMER_A, period1);
	TimerPrescaleSet(WTIMER5_BASE, TIMER_B, extender1);
	TimerLoadSet(WTIMER5_BASE, TIMER_B, period1);
	//set default value A
	TimerPrescaleMatchSet(WTIMER5_BASE, TIMER_A, extender2);
	TimerMatchSet(WTIMER5_BASE, TIMER_A, period2);
	//set default value B
	TimerPrescaleMatchSet(WTIMER5_BASE, TIMER_B, extender2);
	TimerMatchSet(WTIMER5_BASE, TIMER_B, period2);
	TimerEnable(WTIMER5_BASE, TIMER_A);
	TimerEnable(WTIMER5_BASE, TIMER_B);
	set_pwm_width(WTIMER5_BASE, TIMER_A, 50, PULSE_US);
	set_pwm_width(WTIMER5_BASE, TIMER_B, 50, PULSE_US);

	period1 = SysCtlClockGet() / frequency;
	extender1 = period1 >> 16;
	period1 &= 0xFFFF;

	period2 =  SysCtlClockGet() / frequency;
	extender2 = period2 >> 16;
	period2 &= 0xFFFF;

	//Timer 2.A => PF4
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinConfigure(GPIO_PF4_T2CCP0);
	GPIOPinTypeTimer(GPIO_PORTF_BASE, GPIO_PIN_4);
	// Configure timer
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
	TimerConfigure(TIMER2_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM);
	HWREG(TIMER2_BASE + TIMER_O_CTL) |= (TIMER_CTL_TAPWML);
	//set period
	TimerPrescaleSet(TIMER2_BASE, TIMER_A, extender1);
	TimerLoadSet(TIMER2_BASE, TIMER_A, period1);
	//set default value A
	TimerPrescaleMatchSet(TIMER2_BASE, TIMER_A, extender2);
	TimerMatchSet(TIMER2_BASE, TIMER_A, period2);
	TimerEnable(TIMER2_BASE, TIMER_A);
	set_pwm_width(TIMER2_BASE, TIMER_A, 50, PULSE_US);

}

