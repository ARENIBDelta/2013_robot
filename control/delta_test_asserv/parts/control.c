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
#include "../tools/delta_calc.h"

//0 => rien
//1 => init origine
//2 => travaille
unsigned char control_state_1 = 0;
unsigned char control_state_2 = 0;
unsigned int control_t_1 = 0;
unsigned int control_t_2 = 0;

unsigned short qeis[6] = {0, 0, 0, 0, 0, 0};
//static unsigned short qeis_p[6] = {0, 0, 0, 0, 0, 0};
unsigned short errs[6] = {0, 0, 0, 0, 0, 0};

int pwm_pulse_widths[6] = {0, 0, 0, 0, 0, 0};
unsigned short goals[6] = {10200, 10200, 10200, 10200, 10200, 10200};
unsigned char dirs[6] = {1, 1, 1, 1, 1, 1};
unsigned char enabled[6] = {0,0,0,0,0,0};
float errs_a[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
float errs_p[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
float integrals[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
float Kp[6] = {1.75, 1.75, 1.75, 1.75, 1.75, 1.75};
float Ki[6] = {1*0.035, 1*0.035, 1*0.035, 0.035, 0.035, 0.035};
float Kd[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

//Correspondance moteurs et quadratures
unsigned char angles_to_motor[6] = {2,5,4,1,3,6};
unsigned char qeis_to_motor[6] = {4,2,1,5,6,3};

//unsigned short control_next_goals[6] = {10200, 10200, 10200, 10000, 10000, 10000};
unsigned char control_reached_1 = 0;
unsigned char control_reached_2 = 0;
unsigned char control_event = 0;

void motor_set_pwm(unsigned char motor, unsigned char duty_cycle, unsigned char dir) {
	motor = angles_to_motor[motor] - 1;
	switch(motor) {
	case 0:
		set_pwm_width(WTIMER0_BASE, TIMER_B, (100-duty_cycle) * 4, PULSE_100NS);
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, dir ? GPIO_PIN_3 : 0);
		break;
	case 1:
		set_pwm_width(WTIMER1_BASE, TIMER_A, (100-duty_cycle) * 4, PULSE_100NS);
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, dir ? GPIO_PIN_3 : 0);
		break;
	case 2:
		set_pwm_width(WTIMER1_BASE, TIMER_B, (100-duty_cycle) * 4, PULSE_100NS);
		GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, dir ? GPIO_PIN_4 : 0);
		break;
	case 3:
		set_pwm_width(WTIMER5_BASE, TIMER_A, (100-duty_cycle) * 4, PULSE_100NS);
		GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, dir ? GPIO_PIN_4 : 0);
		break;
	case 4:
		set_pwm_width(WTIMER5_BASE, TIMER_B, (100-duty_cycle) * 4, PULSE_100NS);
		GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, dir ? GPIO_PIN_3 : 0);
		break;
	case 5:
		set_pwm_width(TIMER2_BASE,  TIMER_A, duty_cycle * 4, PULSE_100NS);
		GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, dir ? GPIO_PIN_2 : 0);
		break;
	}
}

void control_set_goal_1(unsigned short alpha, unsigned short beta, unsigned short gamma) {
	control_reached_1 = 0;

	goals[0] = alpha;
	goals[1] = beta;
	goals[2] = gamma;
}

void control_set_goal_2(unsigned short alpha, unsigned short beta, unsigned short gamma) {
	control_reached_2 = 0;

	goals[3] = alpha;
	goals[4] = beta;
	goals[5] = gamma;
}

void control_init(void) {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3); //Dir moteur 1
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4);  //Dirs moteur 6, 5, 4
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_3); //Dir moteur 2
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4); //Dir moteur 3

	enabled[0] = 0;
	enabled[1] = 0;
	enabled[2] = 0;
	enabled[3] = 0;
	enabled[4] = 0;
	enabled[5] = 0;

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
	TimerConfigure(TIMER3_BASE, TIMER_CFG_PERIODIC);
	TimerLoadSet(TIMER3_BASE, TIMER_A, SysCtlClockGet()/CONTROL_TIMER_FREQ);
	IntEnable(INT_TIMER3A);
	TimerIntEnable(TIMER3_BASE, TIMER_TIMA_TIMEOUT);

	IntMasterDisable();
	config_pwms();
	IntMasterEnable();

	motor_set_pwm(0, 1, 0);
	motor_set_pwm(1, 1, 0);
	motor_set_pwm(2, 1, 0);
	motor_set_pwm(3, 1, 0);
	motor_set_pwm(4, 1, 0);
	motor_set_pwm(5, 1, 0);
}

void control_enable_1(void) {
	control_reached_1 = 0;
	enabled[0] = 1;
	enabled[1] = 1;
	enabled[2] = 1;
	control_state_1 = 2;
	TimerEnable(TIMER3_BASE, TIMER_A);
}

void control_enable_2(void) {
	control_reached_2 = 0;
	enabled[3] = 1;
	enabled[4] = 1;
	enabled[5] = 1;
	control_state_2 = 2;
	TimerEnable(TIMER3_BASE, TIMER_A);
}

void control_disable_1(void) {
	control_state_1 = 0;

	motor_set_pwm(0, 1, 0);
	motor_set_pwm(1, 1, 0);
	motor_set_pwm(2, 1, 0);

	enabled[0] = 0;
	enabled[1] = 0;
	enabled[2] = 0;
}

void control_disable_2(void) {
	control_state_2 = 0;

	motor_set_pwm(3, 1, 0);
	motor_set_pwm(4, 1, 0);
	motor_set_pwm(5, 1, 0);

	enabled[3] = 0;
	enabled[4] = 0;
	enabled[5] = 0;
}

void control_go_to_origin_1(void) {
	control_reached_1 = 0;
	control_t_1 = 0;
	control_state_1 = 1;

	motor_set_pwm(0, 15, 1);
	motor_set_pwm(1, 15, 1);
	motor_set_pwm(2, 15, 1);
	TimerEnable(TIMER3_BASE, TIMER_A);
}

void control_go_to_origin_2(void) {
	control_t_2 = 0;
	control_state_2 = 1;

	motor_set_pwm(3, 15, 1);
	motor_set_pwm(4, 15, 1);
	motor_set_pwm(5, 15, 1);
	TimerEnable(TIMER3_BASE, TIMER_A);
}

void control_stop_1(void) {
	control_state_1 = 0;
	if (!control_state_1 && !control_state_2)
		TimerDisable(TIMER3_BASE, TIMER_A);
}

void control_stop_2(void) {
	control_state_2 = 0;
	if (!control_state_1 && !control_state_2)
		TimerDisable(TIMER3_BASE, TIMER_A);
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
                       SSI_MODE_MASTER, 75000, 8);
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

	//SPI CS, High -> Low
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, 0);
	SysCtlDelay(10000);

	//reset quadrature
	SSIDataPut(SSI1_BASE, 0xBB);
	SSIDataGet(SSI1_BASE, &in);

	//SPI CS, Low => High
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, GPIO_PIN_2);
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

	//SPI configure
	qeis_spi_reinit();

	//
	// MSP2
	//

	//SPI CS, High -> Low
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, 0);
	SysCtlDelay(10000);

	//récupération des quadratures
	p = ((unsigned char *)&(qeis[3]));
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
	p = ((unsigned char *)&(errs[3]));
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

void control(void) {
	unsigned char i;
	float derr_dt, out;

	//Input
	IntMasterDisable();
	qeis_spi_read();
	IntMasterEnable();

	//Calc
	for(i=6;i--;) {
		unsigned short qei = qeis[qeis_to_motor[i]-1];
		errs_a[i] = goals[i] - qei;
		derr_dt = errs_a[i] - errs_p[i];
		integrals[i] = integrals[i] + Ki[i]*errs_a[i];
		integrals[i] = integrals[i] > 40 ? 40 : integrals[i];
		integrals[i] = integrals[i] < -40 ? -40 : integrals[i];

		out = (errs_a[i] * Kp[i]) + (derr_dt * Kd[i]) + (integrals[i]);

		dirs[i] = out >= 0 ? 0 : 1;
		pwm_pulse_widths[i] = abs(out);
		pwm_pulse_widths[i] = pwm_pulse_widths[i] < 7 ? 7 : pwm_pulse_widths[i];
		pwm_pulse_widths[i] = pwm_pulse_widths[i] > 99 ? 99 : pwm_pulse_widths[i];

		errs_p[i] = errs_a[i];
	}

	if (enabled[0])
		motor_set_pwm(0, pwm_pulse_widths[0], dirs[0]);
	if (enabled[1])
		motor_set_pwm(1, pwm_pulse_widths[1], dirs[1]);
	if (enabled[2])
		motor_set_pwm(2, pwm_pulse_widths[2], dirs[2]);
	if (enabled[3])
		motor_set_pwm(3, pwm_pulse_widths[3], dirs[3]);
	if (enabled[4])
		motor_set_pwm(4, pwm_pulse_widths[4], dirs[4]);
	if (enabled[5])
		motor_set_pwm(5, pwm_pulse_widths[5], dirs[5]);


//	unsigned char threshold = 25;
//	if (enabled[0] && enabled[1] && enabled[2]) {
//		if ((abs(errs_a[0]) < threshold) && (abs(errs_a[1]) < threshold) && (abs(errs_a[2]) < threshold)) {
//			control_reached = 1;
//			control_stop();
//		}
//	}
}

void Timer3IntHandler(void) {
	if (control_state_1 == 1) {
		control_t_1++;
		if (control_t_1 == 600) {
			control_stop_1();
			IntMasterDisable();
			qeis_reset();
			IntMasterEnable();
			control_reached_1 = 1;
		}
	}
	if (control_state_2 == 1) {
		control_t_2++;
		if (control_t_2 == 600) {
			control_stop_2();
			IntMasterDisable();
			qeis_reset();
			IntMasterEnable();
			control_reached_2 = 1;
		}
	}

	if (control_state_1 == 2 || control_state_2 == 2) {
		control_event = 1;
	}

    TimerIntClear(TIMER3_BASE, TIMER_TIMA_TIMEOUT);
}
