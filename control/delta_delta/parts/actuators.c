#define PART_LM4F120B2QR
#include <stdint.h>
#include <inc/hw_types.h>
#include <inc/hw_memmap.h>
#include <inc/hw_gpio.h>
#include <inc/hw_ints.h>
#include <inc/hw_timer.h>
#include <driverlib/sysctl.h>
#include <driverlib/timer.h>
#include <driverlib/adc.h>
#include <driverlib/pin_map.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/interrupt.h>

//
// PWM
//

#define tics_us SysCtlClockGet() / 1000000
#define tics_100ns SysCtlClockGet() / 10000000
#define tics_ms SysCtlClockGet() / 1000
#define default_pulse 0
#define PULSE_US 1
#define PULSE_100NS 2
#define PULSE_MS 3

static void pwm_init(void) {
	uint32_t frequency = 50;

	uint32_t period1 = SysCtlClockGet() / frequency;
	uint32_t extender1 = period1 >> 16;
	period1 &= 0xFFFF;

	uint32_t period2 =  SysCtlClockGet() / frequency;
	uint32_t extender2 = period2 >> 16;
	period2 &= 0xFFFF;

	//PB6 + PB7
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinConfigure(GPIO_PB7_T0CCP1);
	GPIOPinConfigure(GPIO_PB6_T0CCP0);
	GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_7 | GPIO_PIN_6);
	// Configure timer
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM|TIMER_CFG_B_PWM);
	HWREG(TIMER0_BASE + TIMER_O_CTL) |= (TIMER_CTL_TAPWML | TIMER_CTL_TBPWML);
	//set period
	TimerPrescaleSet(TIMER0_BASE, TIMER_A, extender1);
	TimerLoadSet(TIMER0_BASE, TIMER_A, period1);
	//set default value A
	TimerPrescaleMatchSet(TIMER0_BASE, TIMER_A, extender2);
	TimerMatchSet(TIMER0_BASE, TIMER_A, period2);
	//set period
	TimerPrescaleSet(TIMER0_BASE, TIMER_B, extender1);
	TimerLoadSet(TIMER0_BASE, TIMER_B, period1);
	//set default value A
	TimerPrescaleMatchSet(TIMER0_BASE, TIMER_B, extender2);
	TimerMatchSet(TIMER0_BASE, TIMER_B, period2);

	TimerEnable(TIMER0_BASE, TIMER_A);
	TimerEnable(TIMER0_BASE, TIMER_B);
}

static void pwm_set_width(unsigned char index, uint32_t pulse_duration, uint8_t pulse_unit) {
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

	   uint16_t pwm_extender = pwm_period >> 16;
	   pwm_period &= 0xFFFF;
	   //uint8_t pwm_extender = 0;

	   switch(index) {
	   case 2:
		   TimerPrescaleMatchSet(TIMER2_BASE, TIMER_B, pwm_extender);
		   TimerMatchSet(TIMER2_BASE, TIMER_B, pwm_period);
		   break;
	   case 1:
		   TimerPrescaleMatchSet(TIMER0_BASE, TIMER_A, pwm_extender);
		   TimerMatchSet(TIMER0_BASE, TIMER_A, pwm_period);
		   break;
	   case 0:
	   default:
		   TimerPrescaleMatchSet(TIMER0_BASE, TIMER_B, pwm_extender);
	       TimerMatchSet(TIMER0_BASE, TIMER_B, pwm_period);
		   break;
	   }
}

#define MAX 19300
#define MIN 17750
#define ACCELERATION_INCR 20
#define MAX_SPEED 100
static unsigned char states[2] = {0, 0};
static unsigned int goals[2] = {19400, 19400};
static int speeds[2] = {200, 200};
static unsigned int pos[3] = {17650, 17650};

static void step(void) {
	unsigned char i;
	for (i=2; i--;) {
		if (states[i] == 1) {
			pos[i] = pos[i] + speeds[i];
			if (pos[i] >= goals[i])
				states[i] = 0;
		}
		else if (states[i] == 2) {
			pos[i] = pos[i] - speeds[i];
			if (pos[i] <= goals[i])
				states[i] = 0;
		}
		if (pos[i] > MAX)
			pos[i] = MAX;
		if (pos[i] < MIN)
			pos[i] = MIN;
		pwm_set_width(i, pos[i], PULSE_US);
	}
}

static void set_goal(unsigned char index, unsigned int g) {
	goals[index] = g;
	speeds[index] = 50;
	if (g > pos[index])
		states[index] = 1;
	else if (g < pos[index])
		states[index] = 2;
}

void actuators_servos_init(void) {
	pwm_init();
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER4);
	TimerConfigure(TIMER4_BASE, TIMER_CFG_A_PERIODIC | TIMER_CFG_B_PERIODIC | TIMER_CFG_SPLIT_PAIR); //Timer A pour la tourelle
	TimerLoadSet(TIMER4_BASE, TIMER_B, SysCtlClockGet()/50);
	IntEnable(INT_TIMER4B);
	TimerIntEnable(TIMER4_BASE, TIMER_TIMB_TIMEOUT);
	TimerEnable(TIMER4_BASE, TIMER_B);
}

const unsigned short angles_lower[3] = {270, 90, 270};
const unsigned short angles_risen[3] = {90, 270, 90};

void actuators_servo_raise(unsigned char index) {
	float angle = angles_risen[index];
	float goal = MIN + ((float)angle / 360 * (MAX - MIN));
	set_goal(index, goal);
}

void actuators_servo_lower(unsigned char index) {
	float angle = angles_lower[index];
	float goal = MIN + ((float)angle / 360 * (MAX - MIN));
	set_goal(index, goal);
}

void Timer4BIntHandler(void) {
	step();
	TimerIntClear(TIMER4_BASE, TIMER_TIMB_TIMEOUT);
}

void actuator_pwm_init(void) {
	uint32_t frequency = 30000;

	uint32_t period1 = SysCtlClockGet() / frequency;
	uint32_t extender1 = period1 >> 16;
	period1 &= 0xFFFF;

	uint32_t period2 =  SysCtlClockGet() / frequency;
	uint32_t extender2 = period2 >> 16;
	period2 &= 0xFFFF;

	//PB1
	// Configure timer
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
	TimerConfigure(TIMER2_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM|TIMER_CFG_B_PWM); //TIMER_CFG_A_PWM pour les moteurs
	HWREG(TIMER2_BASE + TIMER_O_CTL) |= (TIMER_CTL_TBPWML);
	//set period
	TimerPrescaleSet(TIMER2_BASE, TIMER_B, extender1);
	TimerLoadSet(TIMER2_BASE, TIMER_B, period1);
	//set default value B
	TimerPrescaleMatchSet(TIMER2_BASE, TIMER_B, extender2);
	TimerMatchSet(TIMER2_BASE, TIMER_B, period2);
	TimerEnable(TIMER2_BASE, TIMER_A);
	TimerDisable(TIMER2_BASE, TIMER_B);
	//TimerEnable(TIMER2_BASE, TIMER_B);
}

void actuator_pwm_zero(void) {
	TimerDisable(TIMER2_BASE, TIMER_B);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_1);
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, GPIO_PIN_1);
}

void actuator_pwm_50percent(void) {
	pwm_set_width(2, 33-16, PULSE_US);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinConfigure(GPIO_PB1_T2CCP1);
	GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_1);
	TimerEnable(TIMER2_BASE, TIMER_B);
}
void actuator_pwm_30percent(void) {
	pwm_set_width(2, 33-10, PULSE_US);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinConfigure(GPIO_PB1_T2CCP1);
	GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_1);
	TimerEnable(TIMER2_BASE, TIMER_B);
}
