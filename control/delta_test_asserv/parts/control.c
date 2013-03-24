#define PART_LM4F120B2QR
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/ssi.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "math.h"

#include "../platform/pwm.h"
#include "control.h"

//0 => rien
//1 => init origine
//2 => travaille
unsigned char control_state = 0;
unsigned int control_t = 0;

unsigned short qeis[6] = {0, 0, 0, 0, 0, 0};
unsigned short qeis_p[6] = {0, 0, 0, 0, 0, 0};
unsigned short errs[6] = {0, 0, 0, 0, 0, 0};

int pwm_pulse_widths[6] = {0, 0, 0, 0, 0, 0};
unsigned short goals[6] = {10075, 10075, 10075, 10075, 10075, 10075};
unsigned char dirs[6] = {1, 1, 1, 1, 1, 1};
unsigned char enabled[6] = {1, 1, 1, 1, 1, 1};
float errs_p[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
float integrals[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
float Kp[6] = {1, 1, 1, 1, 1, 1};
float Ki[6] = {0.001, 0.001, 0.001, 0.01, 0.01, 0.01};
float Kd[6] = {0.0, 0.0, 0.0, 42.0, 42.0, 42.0};

float t;

void control_init(void) {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3); //Dir moteur 1
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_6 | GPIO_PIN_4);  //Dirs moteur 6, 5, 4
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_3); //Dir moteur 2
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4); //Dir moteur 3

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
	TimerConfigure(TIMER3_BASE, TIMER_CFG_PERIODIC);
	TimerLoadSet(TIMER3_BASE, TIMER_A, SysCtlClockGet()/CONTROL_TIMER_FREQ);
	IntEnable(INT_TIMER3A);
	TimerIntEnable(TIMER3_BASE, TIMER_TIMA_TIMEOUT);

	IntMasterDisable();
	config_pwms();
	IntMasterEnable();
}

void control_start(void) {
	control_state = 2;
	TimerEnable(TIMER3_BASE, TIMER_A);
}

void control_stop(void) {
	TimerDisable(TIMER3_BASE, TIMER_A);

	set_pwm_width(WTIMER0_BASE, TIMER_B, 100 * 2, PULSE_100NS);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);

	set_pwm_width(WTIMER1_BASE, TIMER_A, 100 * 2, PULSE_100NS);
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, GPIO_PIN_3);

	set_pwm_width(WTIMER1_BASE, TIMER_B, 100 * 2, PULSE_100NS);
	GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_PIN_4);
}

void control_go_to_origin(void) {
	control_t = 0;
	control_state = 1;

	set_pwm_width(WTIMER0_BASE, TIMER_B, 100-15 * 2, PULSE_100NS);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);

	set_pwm_width(WTIMER1_BASE, TIMER_A, 100-15 * 2, PULSE_100NS);
	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, GPIO_PIN_3);

	set_pwm_width(WTIMER1_BASE, TIMER_B, 100-15 * 2, PULSE_100NS);
	GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_PIN_4);

	TimerEnable(TIMER3_BASE, TIMER_A);
}

void qeis_spi_reinit(void) {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2, GPIO_PIN_1 | GPIO_PIN_2);

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

void qeis_reset(void) {
	unsigned long in;

	//SPI configure
	qeis_spi_reinit();

	//SPI CS, High -> Low
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, 0);
	SysCtlDelay(10000);

	//reset quadrature
	SSIDataPut(SSI1_BASE, 0xBB);
	SSIDataGet(SSI1_BASE, &in);

	//SPI CS, Low => High
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, GPIO_PIN_1);
}

void qeis_spi_read(void) {
	unsigned char *p;
	unsigned long in;

	//SPI configure
	qeis_spi_reinit();


	//
	// MSP1
	//

	//SPI CS, High -> Low
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, 0);
	SysCtlDelay(10000);

	//récupération des quadratures
	p = (unsigned char *)qeis;
	SSIDataPut(SSI1_BASE, 0x55);
	SSIDataGet(SSI1_BASE, &in);
	//SysCtlDelay(1000);
	unsigned char i;
	for (i=6;i--;) {
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
	for (i=6;i--;) {
		SSIDataPut(SSI1_BASE, 0x00);
		SSIDataGet(SSI1_BASE, &in);
		*(p++) = in & 0xFF;
		//SysCtlDelay(1000);
	}

	//SPI CS, Low => High
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, GPIO_PIN_1);

	//
	// MSP2
	//

	//SPI CS, High -> Low
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, 0);
	SysCtlDelay(10000);

	//récupération des quadratures
	p = ((unsigned char *)qeis + 6);
	SSIDataPut(SSI1_BASE, 0x55);
	SSIDataGet(SSI1_BASE, &in);
	//SysCtlDelay(1000);
	for (i=6;i--;) {
		SSIDataPut(SSI1_BASE, 0x00);
		SSIDataGet(SSI1_BASE, &in);
		*(p++) = in & 0xFF;
		//SysCtlDelay(1000);
	}

	//récupération des erreurs
	p = ((unsigned char *)errs + 6);
	SSIDataPut(SSI1_BASE, 0xAA);
	SSIDataGet(SSI1_BASE, &in);
	//SysCtlDelay(1000);
	for (i=6;i--;) {
		SSIDataPut(SSI1_BASE, 0x00);
		SSIDataGet(SSI1_BASE, &in);
		*(p++) = in & 0xFF;
		//SysCtlDelay(1000);
	}

	//SPI CS, Low => High
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, GPIO_PIN_2);
}

#define M_PI 3.14159265358979323846

void control(void) {
	unsigned char i;
	float err, derr_dt, out;

	//Input
	IntMasterDisable();
	qeis_spi_read();
	IntMasterEnable();

	t += 2 * M_PI / 500;
	goals[0] = 10000 + 75*cos(t);
	goals[1] = 10000 + 75*cos(t);
	goals[2] = 10000 + 75*cos(t);

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
		pwm_pulse_widths[i] = pwm_pulse_widths[i] > 30 ? 30 : pwm_pulse_widths[i];

		errs_p[i] = err;
	}

	//Output
	if (enabled[0]) {
		set_pwm_width(WTIMER0_BASE, TIMER_B, 100-pwm_pulse_widths[0] * 2, PULSE_100NS);
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, dirs[0] ? GPIO_PIN_3 : 0);
	}
	if (enabled[1]) {
		set_pwm_width(WTIMER1_BASE, TIMER_A, 100-pwm_pulse_widths[1] * 2, PULSE_100NS);
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, dirs[1] ? GPIO_PIN_3 : 0);
	}
	if (enabled[2]) {
		set_pwm_width(WTIMER1_BASE, TIMER_B, 100-pwm_pulse_widths[2] * 2, PULSE_100NS);
		GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, dirs[2] ? GPIO_PIN_4 : 0);
	}
	if (enabled[3]) {
		set_pwm_width(WTIMER5_BASE, TIMER_A, 100-pwm_pulse_widths[3] * 2, PULSE_100NS);
		GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, dirs[3] ? GPIO_PIN_4 : 0);
	}
	if (enabled[4]) {
		set_pwm_width(WTIMER5_BASE, TIMER_B, 100-pwm_pulse_widths[4] * 2, PULSE_100NS);
		GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_6, dirs[4] ? GPIO_PIN_6 : 0);
	}
	if (enabled[5]) {
		set_pwm_width(TIMER2_BASE,  TIMER_A, pwm_pulse_widths[5] * 2, PULSE_100NS);
		GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, dirs[5] ? GPIO_PIN_2 : 0);
	}
}

void Timer3IntHandler(void) {
	switch(control_state) {
	case 1:
		control_t++;
		if (control_t == 2500) {
			control_stop();
			IntMasterDisable();
			qeis_reset();
			IntMasterEnable();
			control_start();
		}
		break;
	case 2:
		control();
		break;
	default:
		break;
	}
    TimerIntClear(TIMER3_BASE, TIMER_TIMA_TIMEOUT);
}
