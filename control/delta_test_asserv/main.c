/*
 * Test d'asservissement d'un moteurs CC
 * 2013-03 Erwan Martin <erwan@martin.tl>
 *
 * Ce montage utilise un MSP-EXP430G2 pour récupérer la valeur des
 * codeurs, et un driver H pour commander le moteur.
 * Ne pas oublier de mettre des résistances de tirage de 10k environ
 * pour charger les sorties codeurs du moteur.
 *
 * Pineout SPI:
 * EK-LM4F120X  -> MSP-EXP430G2
 * PD0 (SSICLK) -> P1.4 (UCA0CLK)
 * PD2 (SSIRX)  -> P1.1 (UCA0SOMI)
 * PD3 (SSITX)  -> P1.2 (UCA0SIMO)
 * PE1 (PE1)    -> P1.5 (P1.5)
 * PE2 (PE2)    -> P2.0 (P2.0)
 * PE3 (PE3)    -> P2.1 (P2.1)
 *
 * Pineout H:
 * VCC          -> VCC Codeurs
 * PB4          -> PWM Moteur 1
 * PE2          -> Dir Moteur 1
 */

#define PART_LM4F120B2QR
#include <inc/hw_types.h>
#include "driverlib/fpu.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/pin_map.h"
#include "inc/hw_ints.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include <stdint.h>
#include <stdlib.h>
#include "driverlib/timer.h"

#include "parts/control.h"
#include "platform/pwm.h"
#include "platform/uartbt.h"
#include "tools/delta_calc.h"

#include "math.h"
#define M_PI 3.14159265358979323846
extern unsigned short next_goals[6];
extern unsigned char control_reached_1;
extern unsigned char control_reached_2;

static float points_x[175] = {0,0.080866,0.32271,0.72331,1.279,1.9845,2.8335,3.8179,4.9289,6.156,7.488,8.9124,10.416,11.986,13.606,15.262,16.939,18.621,20.293,21.939,23.544,25.093,26.572,27.967,29.265,30.455,31.525,32.465,33.267,33.923,34.428,34.776,34.964,34.991,34.856,34.561,34.108,33.502,32.748,31.853,30.825,29.674,28.411,27.047,25.594,24.067,22.479,20.845,19.18,17.5,17.5,17.484,17.437,17.358,17.248,17.107,16.935,16.733,16.5,16.238,15.946,15.626,15.278,14.902,14.499,14.07,13.616,13.137,12.634,12.109,11.562,10.994,10.406,9.7996,9.1754,8.5347,7.8786,7.2083,6.525,5.8299,5.1244,4.4096,3.6869,2.9575,2.2228,1.4841,0.74272,1.0716e-15,-0.74272,-1.4841,-2.2228,-2.9575,-3.6869,-4.4096,-5.1244,-5.8299,-6.525,-7.2083,-7.8786,-8.5347,-9.1754,-9.7996,-10.406,-10.994,-11.562,-12.109,-12.634,-13.137,-13.616,-14.07,-14.499,-14.902,-15.278,-15.626,-15.946,-16.238,-16.5,-16.733,-16.935,-17.107,-17.248,-17.358,-17.437,-17.484,-17.5,-17.5,-19.18,-20.845,-22.479,-24.067,-25.594,-27.047,-28.411,-29.674,-30.825,-31.853,-32.748,-33.502,-34.108,-34.561,-34.856,-34.991,-34.964,-34.776,-34.428,-33.923,-33.267,-32.465,-31.525,-30.455,-29.265,-27.967,-26.572,-25.093,-23.544,-21.939,-20.293,-18.621,-16.939,-15.262,-13.606,-11.986,-10.416,-8.9124,-7.488,-6.156,-4.9289,-3.8179,-2.8335,-1.9845,-1.279,-0.72331,-0.32271,-0.080866,0};
static float points_y[175] = {-35,-36.68,-38.345,-39.979,-41.567,-43.094,-44.547,-45.911,-47.174,-48.325,-49.353,-50.248,-51.002,-51.608,-52.061,-52.356,-52.491,-52.464,-52.276,-51.928,-51.423,-50.767,-49.965,-49.025,-47.955,-46.765,-45.467,-44.072,-42.593,-41.044,-39.439,-37.793,-36.121,-34.439,-32.762,-31.106,-29.486,-27.916,-26.412,-24.988,-23.656,-22.429,-21.318,-20.333,-19.485,-18.779,-18.223,-17.823,-17.581,-17.5,-17.5,-14.529,-11.564,-8.6088,-5.6699,-2.7525,0.13846,2.9976,5.8198,8.6,11.333,14.014,16.639,19.202,21.698,24.124,26.475,28.747,30.936,33.037,35.047,36.963,38.78,40.496,42.107,43.611,45.005,46.286,47.452,48.501,49.432,50.241,50.929,51.493,51.933,52.248,52.437,52.5,52.437,52.248,51.933,51.493,50.929,50.241,49.432,48.501,47.452,46.286,45.005,43.611,42.107,40.496,38.78,36.963,35.047,33.037,30.936,28.747,26.475,24.124,21.698,19.202,16.639,14.014,11.333,8.6,5.8198,2.9976,0.13846,-2.7525,-5.6699,-8.6088,-11.564,-14.529,-17.5,-17.5,-17.581,-17.823,-18.223,-18.779,-19.485,-20.333,-21.318,-22.429,-23.656,-24.988,-26.412,-27.916,-29.486,-31.106,-32.762,-34.439,-36.121,-37.793,-39.439,-41.044,-42.593,-44.072,-45.467,-46.765,-47.955,-49.025,-49.965,-50.767,-51.423,-51.928,-52.276,-52.464,-52.491,-52.356,-52.061,-51.608,-51.002,-50.248,-49.353,-48.325,-47.174,-45.911,-44.547,-43.094,-41.567,-39.979,-38.345,-36.68,-35};

char vvvvvvv = 0;
void main(void) {
	//Config sys
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);

	FPUEnable();
	FPULazyStackingEnable();

    //configure_uart_bt();

    config_pwms();
    control_init();

//	unsigned char dir = 1;
//    unsigned char pwm = 15;
//    while(1) {
//		set_pwm_width(WTIMER0_BASE, TIMER_B, (100-pwm) * 4, PULSE_100NS);
//		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, dir ? GPIO_PIN_3 : 0);
//
//		set_pwm_width(WTIMER0_BASE, TIMER_B, (100-1) * 4, PULSE_100NS);
//		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, dir ? GPIO_PIN_3 : 0);
//
//
//
//		set_pwm_width(WTIMER1_BASE, TIMER_A, (100-pwm) * 4, PULSE_100NS);
//		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, dir ? GPIO_PIN_3 : 0);
//
//		set_pwm_width(WTIMER1_BASE, TIMER_A, (100-1) * 4, PULSE_100NS);
//		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, dir ? GPIO_PIN_3 : 0);
//
//
//
//		set_pwm_width(WTIMER1_BASE, TIMER_B, (100-pwm) * 4, PULSE_100NS);
//		GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, dir ? GPIO_PIN_4 : 0);
//
//		set_pwm_width(WTIMER1_BASE, TIMER_B, (100-1) * 4, PULSE_100NS);
//		GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, dir ? GPIO_PIN_4 : 0);
//
//
//
//    	set_pwm_width(WTIMER5_BASE, TIMER_A, (100-pwm) * 4, PULSE_100NS);
//    	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, dir ? GPIO_PIN_4 : 0);
//
//    	set_pwm_width(WTIMER5_BASE, TIMER_A, (100-1) * 4, PULSE_100NS);
//    	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, dir ? GPIO_PIN_4 : 0);
//
//
//
//    	set_pwm_width(WTIMER5_BASE, TIMER_B, (100-pwm) * 4, PULSE_100NS);
//    	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, dir ? GPIO_PIN_3 : 0);
//
//    	set_pwm_width(WTIMER5_BASE, TIMER_B, (100-1) * 4, PULSE_100NS);
//    	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, dir ? GPIO_PIN_3 : 0);
//
//
//
//    	set_pwm_width(TIMER2_BASE,  TIMER_A, pwm * 4, PULSE_100NS);
//    	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, dir ? GPIO_PIN_2 : 0);
//
//    	set_pwm_width(TIMER2_BASE,  TIMER_A, 1 * 4, PULSE_100NS);
//    	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, dir ? GPIO_PIN_2 : 0);
//    }

//    //Init quadrature
//    control_go_to_origin_1();
//	while(!control_reached_1) {
//		SysCtlDelay(1000);
//	}
//	control_disable_1();
//	control_stop_1();
//
//    control_enable_1();

    //Init quadrature
    control_go_to_origin_2();
	while(!control_reached_2) {
		SysCtlDelay(1000);
	}
	control_disable_2();
	control_stop_2();

    control_enable_2();

	float t = 0;
	char pi = 0;
	unsigned short alpha_a, beta_a, gamma_a;
	while(1) {
		delta_params params;
		params.a = 70;
		params.b = 225;
		params.c = 129.8;
		params.d = 143.777;
		float alpha, beta, gamma;
		float X, Y, Z;

		//pi++;
		//pi = pi % 175;

		//X = points_x[pi];
		//Y = points_y[pi];
		//Z = 200;

		t += 2 * M_PI / 1000;
		X = 35*cos(t);
		Y = 35*sin(t);
		Z = 200;
		//Z = 200 + 20 * cos(t);

		//Z = 220 + 20 * cos(t);

		//Ne pas oublier de vérifier la taille de la stack avant d'utiliser ça
		delta_calc(params, X, Y, Z, 1, &alpha, &beta, &gamma);
		alpha_a = 10245 - ((alpha / M_PI * 180) )/0.108; //90/(10546-9719)
		beta_a = 10245 - ((beta / M_PI * 180) )/0.108;
		gamma_a = 10245 - ((gamma / M_PI * 180) )/0.108;

		alpha_a = alpha_a > 10700 ? 10700 : alpha_a;
		alpha_a = alpha_a < 9800 ? 9800 : alpha_a;
		beta_a = beta_a > 10700 ? 10700 : beta_a;
		beta_a = beta_a < 9800 ? 9800 : beta_a;
		gamma_a = gamma_a > 10700 ? 10700 : gamma_a;
		gamma_a = gamma_a < 9800 ? 9800 : gamma_a;

		//UARTprintf("%d %d %d\r\n", alpha_a, beta_a, gamma_a);

//		while(!control_reached) {
//			SysCtlDelay(1000);
//		}

		SysCtlDelay(50000000/1000);

		control_set_goal_2(alpha_a, beta_a, gamma_a);
		//control_start();

		//la patience est une vertue qui s'aquiert avec de la patience
		//SysCtlDelay(1000);
	}

}
