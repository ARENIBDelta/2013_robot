#ifndef PCCONTROL_H_
#define PCCONTROL_H_

#define PCCONTROL_GETQEIS 10
#define PCCONTROL_GETQERRS 11
#define PCCONTROL_GETGOALS 12
#define PCCONTROL_GETENABLEDS 13

#define PCCONTROL_SETQEIS 20
#define PCCONTROL_SETQERRS 21
#define PCCONTROL_SETGOALS 22
#define PCCONTROL_SETENABLEDS 23

typedef struct _pccontrol_frompc {
	unsigned char command;
	unsigned int data[6];
} pccontrol_frompc;

typedef struct _pccontrol_topc {
	unsigned char command;
	unsigned int data[6];
} pccontrol_to_pc;

void pccontrol_init(void);


#endif /* PCCONTROL_H_ */
