#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/uart.h"

#include "../tools/layer2p.h"
#include "pccontrol.h"

layer2p_ctx pccontrol_l2p;

extern unsigned short qeis[6];
extern unsigned short qeis_p[6];
extern unsigned short errs[6];

extern int pwm_pulse_widths[6];
extern unsigned short goals[6];
extern unsigned char dirs[6];
extern unsigned char enabled[6];
extern float errs_p[6];
extern float integrals[6];
extern float Kp[6];
extern float Ki[6];
extern float Kd[6];

void UART5IntHandler(void) {
    unsigned long ulStatus;
    ulStatus = UARTIntStatus(UART5_BASE, 1);
    UARTIntClear(UART0_BASE, ulStatus);

    while(UARTCharsAvail(UART5_BASE)) {
    	layer2p_receive(&pccontrol_l2p, UARTCharGet(UART5_BASE));
    }
}

void pccontrol_linksend(unsigned char data, unsigned char lastbyte) {
	UARTCharPut(UART5_BASE, data);
}

void pccontrol_received(unsigned char *data, unsigned int size) {
	pccontrol_to_pc odata;
	unsigned char i;
	pccontrol_frompc *sdata = (pccontrol_frompc *) data;

	switch( sdata->command ) {
	case PCCONTROL_GETQEIS:
		odata.command = PCCONTROL_GETQEIS;
		for (i=6; i--; )
			odata.data[i] = qeis[i];
		layer2p_send(&pccontrol_l2p, (unsigned char *)&odata, sizeof(odata));
		break;
	case PCCONTROL_GETQERRS:
		odata.command = PCCONTROL_GETQERRS;
		for (i=6; i--; )
			odata.data[i] = errs[i];
		layer2p_send(&pccontrol_l2p, (unsigned char *)&odata, sizeof(odata));
		break;
	case PCCONTROL_GETGOALS:
		odata.command = PCCONTROL_GETGOALS;
		for (i=6; i--; )
			odata.data[i] = goals[i];
		layer2p_send(&pccontrol_l2p, (unsigned char *)&odata, sizeof(odata));
		break;
	case PCCONTROL_GETENABLEDS:
		odata.command = PCCONTROL_GETENABLEDS;
		for (i=6; i--; )
			odata.data[i] = enabled[i];
		layer2p_send(&pccontrol_l2p, (unsigned char *)&odata, sizeof(odata));
		break;
	default:
		break;
	}
}

void pccontrol_errored(void) {

}

void pccontrol_init(void) {
	layer2p_ctx_init(&pccontrol_l2p);
	layer2p_set_linksend_callback(&pccontrol_l2p, pccontrol_linksend);
	layer2p_set_callback(&pccontrol_l2p, pccontrol_received);
	layer2p_set_error_callback(&pccontrol_l2p, pccontrol_errored);
}


