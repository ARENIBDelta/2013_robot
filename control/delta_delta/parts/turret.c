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

void pwm_init(void) {
	uint32_t frequency = 50;

	uint32_t period1 = SysCtlClockGet() / frequency;
	uint32_t extender1 = period1 >> 16;
	period1 &= 0xFFFF;

	uint32_t period2 =  SysCtlClockGet() / frequency;
	uint32_t extender2 = period2 >> 16;
	period2 &= 0xFFFF;

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinConfigure(GPIO_PB2_T3CCP0);
	GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_2);
	// Configure timer
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
	TimerConfigure(TIMER3_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM);
	HWREG(TIMER3_BASE + TIMER_O_CTL) |= (TIMER_CTL_TAPWML);
	//set period
	TimerPrescaleSet(TIMER3_BASE, TIMER_A, extender1);
	TimerLoadSet(TIMER3_BASE, TIMER_A, period1);
	//set default value A
	TimerPrescaleMatchSet(TIMER3_BASE, TIMER_A, extender2);
	TimerMatchSet(TIMER3_BASE, TIMER_A, period2);
	TimerEnable(TIMER3_BASE, TIMER_A);
}

void pwm_set_width(uint32_t pulse_duration, uint8_t pulse_unit) {
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

	   TimerPrescaleMatchSet(TIMER3_BASE, TIMER_A, pwm_extender);
       TimerMatchSet(TIMER3_BASE, TIMER_A, pwm_period);
}

#define MAX 19300
#define MIN 17750
#define ACCELERATION_INCR 20
#define MAX_SPEED 100
unsigned char state = 0;
unsigned int goal = 19400;
int speed = 200;
unsigned int pos = 17650;

void step(void) {
	if (state == 1) {
//		if (pos > goal - 200) {
//			speed = speed - 40;
//		}
//		else {
//			speed = speed + 40;
//		}
//		speed = speed < 0 ? 0 : speed;
//		speed = speed > MAX_SPEED ? MAX_SPEED : speed;
		pos = pos + speed;
		if (pos >= goal)
			state = 0;
	}
	else if (state == 2) {
//		if (pos < goal + 200) {
//			speed = speed - 40;
//		}
//		else {
//			speed = speed + 40;
//		}
//		speed = speed < 0 ? 0 : speed;
//		speed = speed > MAX_SPEED ? MAX_SPEED : speed;
		pos = pos - speed;
		if (pos <= goal)
			state = 0;
	}
	if (pos > MAX)
		pos = MAX;
	if (pos < MIN)
		pos = MIN;
	pwm_set_width(pos, PULSE_US);
}

void set_goal(unsigned int g) {
	goal = g;
	speed = 50;
	if (g > pos)
		state = 1;
	else if (g < pos)
		state = 2;
}

//
// ADC
//

unsigned long ulADC0_Value[2];

void adc_init(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeADC(GPIO_PORTB_BASE, GPIO_PIN_5);
    GPIOPinTypeADC(GPIO_PORTB_BASE, GPIO_PIN_4);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    ADCSequenceDisable(ADC0_BASE, 0);

    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH11);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH10 | ADC_CTL_IE | ADC_CTL_END);

    ADCSequenceEnable(ADC0_BASE, 0);
    ADCIntEnable(ADC0_BASE, 0);
    ADCIntClear(ADC0_BASE, 0);
    IntEnable(ADC_INT_SS0);
}

void adc_read() {
    ADCProcessorTrigger(ADC0_BASE, 0);

    while(!ADCIntStatus(ADC0_BASE, 0, false));

    ADCIntClear(ADC0_BASE, 0);
    ADCSequenceDataGet(ADC0_BASE, 0, ulADC0_Value);

    ADCSequenceEnable(ADC0_BASE, 0);
}

void turret_init(void) {
	pwm_init();
	adc_init();

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER4);
	TimerConfigure(TIMER4_BASE, TIMER_CFG_PERIODIC);
	TimerLoadSet(TIMER4_BASE, TIMER_A, SysCtlClockGet()/50);
	IntEnable(INT_TIMER4A);
	TimerIntEnable(TIMER4_BASE, TIMER_TIMA_TIMEOUT);
	TimerEnable(TIMER4_BASE, TIMER_A);
}

unsigned char turret_read(void) {
	adc_read();
	return (ulADC0_Value[0] < 300) || (ulADC0_Value[1] < 300);
}

void turret_set_angle(unsigned short angle) {
	float goal = MIN + ((float)angle / 360 * (MAX - MIN));
	set_goal(goal);
}


void Timer4IntHandler(void) {
	unsigned char turret_value = turret_read();
	step();
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, turret_value ? GPIO_PIN_2 : 0);
	TimerIntClear(TIMER4_BASE, TIMER_TIMA_TIMEOUT);
}


