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
unsigned short goals[6] = {0, 0, 0, 0, 0, 0};
unsigned char dirs[6] = {0, 0, 0, 0, 0, 0};
unsigned char enabled[6] = {1, 0, 0, 0, 0, 0};

void control_init(void) {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()/CONTROL_TIMER_FREQ);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntEnable(INT_TIMER0A);
	TimerEnable(TIMER0_BASE, TIMER_A);

	IntMasterDisable();
	config_timer_for_pwm(1, 10000);
	set_pwm(2, pwm_pulse_widths[0], PULSE_US);
	IntMasterEnable();
}

void Timer0IntHandler(void) {
	control();
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
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
	short err;

	//Input
	IntMasterDisable();
	qeis_spi_read();
	IntMasterEnable();

	//Calc
	for(i=6;i--;) {
		err = goals[i] - qeis[i];
		dirs[i] = err >= 0 ? 0 : 1;
		pwm_pulse_widths[i] = abs(err);
		pwm_pulse_widths[i] = pwm_pulse_widths[i] > 98 ? 98 : pwm_pulse_widths[i];
	}

	//Output
	if (enabled[0]) {
		set_pwm(2, pwm_pulse_widths[0], PULSE_US);
		GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, dirs[0] ? GPIO_PIN_2 : 0);
	}
}
