#define PART_LM4F120B2QR

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "uartprintf.h"

void UART3IntHandler(void) {
    unsigned long ulStatus;
    ulStatus = UARTIntStatus(UART3_BASE, true);
    UARTIntClear(UART3_BASE, ulStatus);

    //Echo back
    while(UARTCharsAvail(UART3_BASE)) {
    	unsigned char data = UARTCharGetNonBlocking(UART3_BASE);
        UARTCharPutNonBlocking(UART3_BASE, data);
    }
}

int main(void) {
    FPUEnable();
    FPULazyStackingEnable();

    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);

    configure_uart_printf();

    IntMasterEnable();

    unsigned int t = 0;
    while(1) {
    	//UARTCharPut(UART3_BASE, 'E');
    	UARTprintf("compteur: %d\r\n", t++);
    	SysCtlDelay(1000000);
    }
}
