#include <inc/hw_types.h>
#include <inc/hw_memmap.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>

void init_init(void) {
	//GPIO INIT 1
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_3);
	//GPIO INIT 2
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_1);
	//GPIO INIT 3
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_7);
}

unsigned char init_is_blue(void) {
	return GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_7) ? 0 : 1;
}

unsigned char init_is_ready(void) {
	return GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_3) ? 0 : 1;
}

unsigned char init_tirette_go(void) {
	return GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_1) ? 0 : 1;
}
