#define PART_LM4F120B2QR
#include "inc/hw_timer.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include <stdint.h>

#include "pwm.h"

void config_timer_for_pwm(uint8_t timer, uint32_t frequency) {
   uint32_t period1 = SysCtlClockGet() / frequency;
   uint8_t extender1 = period1 >> 16;
   period1 &= 0xFFFF;

   uint32_t period2 =  SysCtlClockGet() / frequency;
   uint8_t extender2 = period2 >> 16;
   period2 &= 0xFFFF;

   switch (timer){
	   case 0: {
		  // Configure output, PB6/7 as T0CCP0/1
		  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
		  GPIOPinConfigure(GPIO_PB0_T2CCP0);
		  GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_6);
		  GPIOPinConfigure(GPIO_PB7_T0CCP1);
		  GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_7);
		  // Configure timer
		  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
		  TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM|TIMER_CFG_B_PWM);
		  HWREG(TIMER0_BASE + TIMER_O_CTL) |= (TIMER_CTL_TAPWML|TIMER_CTL_TBPWML);
		  //set period
		  TimerPrescaleSet(TIMER0_BASE, TIMER_A, extender1);
		  TimerLoadSet(TIMER0_BASE, TIMER_A, period1);
		  TimerPrescaleSet(TIMER0_BASE, TIMER_B, extender1);
		  TimerLoadSet(TIMER0_BASE, TIMER_B, period1);
		  //set default value A
		  TimerPrescaleMatchSet(TIMER0_BASE, TIMER_A, extender2);
		  TimerMatchSet(TIMER0_BASE, TIMER_A, period2);
		  //set default value B
		  TimerPrescaleMatchSet(TIMER0_BASE, TIMER_B, extender2);
		  TimerMatchSet(TIMER0_BASE, TIMER_B, period2);
		  TimerEnable(TIMER0_BASE, TIMER_A|TIMER_B);
		  break;
	   }

	   case 1: {
		   // Configure output, PB4/5 as T1CCP0/1
		  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
		  GPIOPinConfigure(GPIO_PB4_T1CCP0);
		  GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_4);
		  GPIOPinConfigure(GPIO_PB5_T1CCP1);
		  GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_5);
		  // Configure timer
		  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
		  TimerConfigure(TIMER1_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM|TIMER_CFG_B_PWM);
		  HWREG(TIMER1_BASE + TIMER_O_CTL) |= (TIMER_CTL_TAPWML|TIMER_CTL_TBPWML);
		  //set period
		  TimerPrescaleSet(TIMER1_BASE, TIMER_A, extender1);
		  TimerLoadSet(TIMER1_BASE, TIMER_A, period1);
		  TimerPrescaleSet(TIMER1_BASE, TIMER_B, extender1);
		  TimerLoadSet(TIMER1_BASE, TIMER_B, period1);
		  //set default value A
		  TimerPrescaleMatchSet(TIMER1_BASE, TIMER_A, extender2);
		  TimerMatchSet(TIMER1_BASE, TIMER_A, period2);
		  //set default value B
		  TimerPrescaleMatchSet(TIMER1_BASE, TIMER_B, extender2);
		  TimerMatchSet(TIMER1_BASE, TIMER_B, period2);
		  TimerEnable(TIMER1_BASE, TIMER_A|TIMER_B);
		  break;
	   }

	   case 2: {
		   // Configure output, PB0/1 as T2CCP0/1
		  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
		  GPIOPinConfigure(GPIO_PB0_T2CCP0);
		  GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_0);
		  GPIOPinConfigure(GPIO_PB1_T2CCP1);
		  GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_1);
		  // Configure timer
		  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
		  TimerConfigure(TIMER2_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM|TIMER_CFG_B_PWM);
		  HWREG(TIMER2_BASE + TIMER_O_CTL) |= (TIMER_CTL_TAPWML|TIMER_CTL_TBPWML);
		  //set period
		  TimerPrescaleSet(TIMER2_BASE, TIMER_A, extender1);
		  TimerLoadSet(TIMER2_BASE, TIMER_A, period1);
		  TimerPrescaleSet(TIMER2_BASE, TIMER_B, extender1);
		  TimerLoadSet(TIMER2_BASE, TIMER_B, period1);
		  //set default value A
		  TimerPrescaleMatchSet(TIMER2_BASE, TIMER_A, extender2);
		  TimerMatchSet(TIMER2_BASE, TIMER_A, period2);
		  //set default value B
		  TimerPrescaleMatchSet(TIMER2_BASE, TIMER_B, extender2);
		  TimerMatchSet(TIMER2_BASE, TIMER_B, period2);
		  TimerEnable(TIMER2_BASE, TIMER_A|TIMER_B);
		  break;
	   }

	   case 3: {
		  // Configure output, PB2/3 as T3CCP0/1
		  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
		  GPIOPinConfigure(GPIO_PB2_T3CCP0);
		  GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_2);
		  GPIOPinConfigure(GPIO_PB3_T3CCP1);
		  GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_3);
		  // Configure timer
		  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
		  TimerConfigure(TIMER3_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM|TIMER_CFG_B_PWM);
		  HWREG(TIMER3_BASE + TIMER_O_CTL) |= (TIMER_CTL_TAPWML|TIMER_CTL_TBPWML);
		  //set period
		  TimerPrescaleSet(TIMER3_BASE, TIMER_A, extender1);
		  TimerLoadSet(TIMER3_BASE, TIMER_A, period1);
		  TimerPrescaleSet(TIMER3_BASE, TIMER_B, extender1);
		  TimerLoadSet(TIMER3_BASE, TIMER_B, period1);
		  //set default value A
		  TimerPrescaleMatchSet(TIMER3_BASE, TIMER_A, extender2);
		  TimerMatchSet(TIMER3_BASE, TIMER_A, period2);
		  //set default value B
		  TimerPrescaleMatchSet(TIMER3_BASE, TIMER_B, extender2);
		  TimerMatchSet(TIMER3_BASE, TIMER_B, period2);
		  TimerEnable(TIMER3_BASE, TIMER_A|TIMER_B);
		  break;
	   }

	   default: {
		   break;
	   }
   }
}

void set_pwm (uint8_t chan, uint32_t pulse_duration, uint8_t pulse_unit) {
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
   switch (chan) {
	   case 0: {
		  TimerPrescaleMatchSet(TIMER0_BASE, TIMER_A, pwm_extender);
		  TimerMatchSet(TIMER0_BASE, TIMER_A, pwm_period);
		  break;
	   }
	   case 1: {
		  TimerPrescaleMatchSet(TIMER0_BASE, TIMER_B, pwm_extender);
		  TimerMatchSet(TIMER0_BASE, TIMER_B, pwm_period);
		  break;
	   }
	   case 2: {
		  TimerPrescaleMatchSet(TIMER1_BASE, TIMER_A, pwm_extender);
		  TimerMatchSet(TIMER1_BASE, TIMER_A, pwm_period);
		  break;
	   }
	   case 3: {
		  TimerPrescaleMatchSet(TIMER1_BASE, TIMER_B, pwm_extender);
		  TimerMatchSet(TIMER1_BASE, TIMER_B, pwm_period);
		  break;   }
	   case 4: {
		  TimerPrescaleMatchSet(TIMER2_BASE, TIMER_A, pwm_extender);
		  TimerMatchSet(TIMER2_BASE, TIMER_A, pwm_period);
		  break;
	   }
	   case 5: {
		  TimerPrescaleMatchSet(TIMER2_BASE, TIMER_B, pwm_extender);
		  TimerMatchSet(TIMER2_BASE, TIMER_B, pwm_period);
		  break;
	   }
	   case 6: {
		  TimerPrescaleMatchSet(TIMER3_BASE, TIMER_A, pwm_extender);
		  TimerMatchSet(TIMER3_BASE, TIMER_A, pwm_period);
		  break;
	   }
	   case 7: {
		  TimerPrescaleMatchSet(TIMER3_BASE, TIMER_B, pwm_extender);
		  TimerMatchSet(TIMER3_BASE, TIMER_B, pwm_period);
		  break;
	   }
	   default: {
		   break;
	   }
   }
}


