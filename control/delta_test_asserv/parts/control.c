#define PART_LM4F120B2QR
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/ssi.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

#include "../platform/pwm.h"
#include "control.h"

unsigned short qeis[6] = {0, 0, 0, 0, 0, 0};
unsigned short qeis_p[6] = {0, 0, 0, 0, 0, 0};
unsigned short errs[6] = {0, 0, 0, 0, 0, 0};

int pwm_pulse_widths[6] = {0, 0, 0, 0, 0, 0};
unsigned short goals[6] = {1000, 0, 0, 0, 0, 0};
unsigned char dirs[6] = {1, 1, 1, 1, 1, 1};
unsigned char enabled[6] = {1, 0, 0, 0, 0, 0};
float errs_p[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
float integrals[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
float Kp[6] = {1.5, 0.0, 0.0, 0.0, 0.0, 0.0};
float Ki[6] = {0.05, 0.0, 0.0, 0.0, 0.0, 0.0};
float Kd[6] = {42.0, 0.0, 0.0, 0.0, 0.0, 0.0};

void control_init(void) {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_3); //Dir moteur 1
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_2 | GPIO_PIN_6 | GPIO_PIN_4);  //Dirs moteur 6, 5, 4
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_3); //Dir moteur 2
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_4); //Dir moteur 3

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
	TimerConfigure(TIMER3_BASE, TIMER_CFG_PERIODIC);
	TimerLoadSet(TIMER3_BASE, TIMER_A, SysCtlClockGet()/CONTROL_TIMER_FREQ);
	IntEnable(INT_TIMER3A);
	TimerIntEnable(TIMER3_BASE, TIMER_TIMA_TIMEOUT);
	TimerEnable(TIMER3_BASE, TIMER_A);

	IntMasterDisable();
	config_pwms();
	IntMasterEnable();
}

void Timer3IntHandler(void) {
	control();
    TimerIntClear(TIMER3_BASE, TIMER_TIMA_TIMEOUT);
}

void qeis_spi_reinit(void) {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_1);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, GPIO_PIN_1);

	SysCtlPeripheralReset(SYSCTL_PERIPH_SSI1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    GPIOPinConfigure(GPIO_PD0_SSI1CLK);
    GPIOPinConfigure(GPIO_PD1_SSI1FSS);
    GPIOPinConfigure(GPIO_PD2_SSI1RX);
    GPIOPinConfigure(GPIO_PD3_SSI1TX);
    GPIOPinTypeSSI(GPIO_PORTD_BASE,
    		GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    SSIConfigSetExpClk(SSI1_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_3,
                       SSI_MODE_MASTER, 50000, 8);
    SSIEnable(SSI1_BASE);
}

void qeis_spi_read(void) {
	unsigned char *p;
	unsigned long in;

	//SPI configure
	qeis_spi_reinit();
	//SPI CS, High -> Low
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, 0);
	SysCtlDelay(10000);

	//récupération des quadratures
	p = (unsigned char *)qeis;
	SSIDataPut(SSI1_BASE, 0x55);
	SSIDataGet(SSI1_BASE, &in);
	//SysCtlDelay(1000);
	unsigned char i;
	for (i=12;i--;) {
		SSIDataPut(SSI1_BASE, 0x00);
		SSIDataGet(SSI1_BASE, &in);
		*(p++) = in & 0xFF;
		//SysCtlDelay(1000);
	}

	//récupération des erreurs
	p = (unsigned char *)errs;
	SSIDataPut(SSI1_BASE, 0xAA);
	SSIDataGet(SSI1_BASE, &in);
	//SysCtlDelay(1000);
	for (i=12;i--;) {
		SSIDataPut(SSI1_BASE, 0x00);
		SSIDataGet(SSI1_BASE, &in);
		*(p++) = in & 0xFF;
		//SysCtlDelay(1000);
	}

	//SPI CS, Low => High
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, GPIO_PIN_1);
}

void control(void) {
	unsigned char i;
	float err, derr_dt, out;

	//Input
	IntMasterDisable();
	qeis_spi_read();
	IntMasterEnable();

	//Calc
	for(i=6;i--;) {
		err = goals[i] - qeis[i];
		derr_dt = err - errs_p[i];
		integrals[i] = integrals[i] + Ki[i]*err;
		integrals[i] = integrals[i] > 40 ? 40 : integrals[i];
		integrals[i] = integrals[i] < -40 ? -40 : integrals[i];

		out = (err * Kp[i]) + (derr_dt * Kd[i]) + (integrals[i]);

		dirs[i] = out >= 0 ? 0 : 1;
		pwm_pulse_widths[i] = abs(out);
		pwm_pulse_widths[i] = pwm_pulse_widths[i] > 98 ? 98 : pwm_pulse_widths[i];

		errs_p[i] = err;
	}

	//Output
	if (enabled[0]) {
		set_pwm_width(WTIMER0_BASE, TIMER_B, pwm_pulse_widths[0], PULSE_US);
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, dirs[0] ? GPIO_PIN_3 : 0);
	}
	if (enabled[1]) {
		set_pwm_width(WTIMER1_BASE, TIMER_A, pwm_pulse_widths[1], PULSE_US);
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, dirs[0] ? GPIO_PIN_3 : 0);
	}
	if (enabled[2]) {
		set_pwm_width(WTIMER1_BASE, TIMER_B, pwm_pulse_widths[2], PULSE_US);
		GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, dirs[0] ? GPIO_PIN_4 : 0);
	}
	if (enabled[3]) {
		set_pwm_width(WTIMER5_BASE, TIMER_A, pwm_pulse_widths[3], PULSE_US);
		GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, dirs[0] ? GPIO_PIN_4 : 0);
	}
	if (enabled[4]) {
		set_pwm_width(WTIMER5_BASE, TIMER_B, pwm_pulse_widths[4], PULSE_US);
		GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_6, dirs[0] ? GPIO_PIN_6 : 0);
	}
	if (enabled[5]) {
		set_pwm_width(TIMER2_BASE,  TIMER_A, pwm_pulse_widths[5], PULSE_US);
		GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, dirs[0] ? GPIO_PIN_2 : 0);
	}
}
