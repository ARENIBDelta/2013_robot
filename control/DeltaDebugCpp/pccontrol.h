//---------------------------------------------------------------------------

#ifndef pccontrolH
#define pccontrolH

#include <stdint.h>

#define PCCONTROL_GETQEIS 10
#define PCCONTROL_GETQERRS 11
#define PCCONTROL_GETGOALS 12
#define PCCONTROL_GETENABLEDS 13
#define PCCONTROL_GETKPDIS 14

#define PCCONTROL_SETQEIS 20
#define PCCONTROL_SETQERRS 21
#define PCCONTROL_SETGOALS 22
#define PCCONTROL_SETENABLEDS 23
#define PCCONTROL_SETKPDIS 24

typedef struct _pccontrol_frompc {
	uint8_t command;
	union {
		uint32_t data_i[18];
		float data_f[18];
	} data;
} pccontrol_frompc;

typedef struct _pccontrol_topc {
	uint8_t command;
	union {
		uint32_t data_i[18];
		float data_f[18];
	} data;
} pccontrol_to_pc;

//---------------------------------------------------------------------------
#endif
