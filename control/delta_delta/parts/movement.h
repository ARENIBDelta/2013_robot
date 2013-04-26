#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include <inc/hw_types.h>
#include <driverlib/sysctl.h>

#include "../tools/delta_calc.h"
#include "control.h"

#define STEPS    50
#define BASE_Z   200
#define DZ       30

extern unsigned short next_goals[6];
extern unsigned char  control_reached_1;
extern unsigned char  control_reached_2;
extern unsigned char  control_event;

void control_do_lines(
		float X1_from, float X1_to,
		float Y1_from, float Y1_to,
		float Z1_from, float Z1_to,
		float X2_from, float X2_to,
		float Y2_from, float Y2_to,
		float Z2_from, float Z2_to,
		short steps
);
void          control_start_steps(float angle, unsigned char length, unsigned char step_down);
void          control_start_steps2(float angle, unsigned char length);
unsigned char control_do_step(float angle_src, float angle_dst, char length);
unsigned char control_do_half_step(float angle_src, float angle_dst, char length);
void          control_stop_steps(float angle, unsigned char length, unsigned char step_down);
void          movement_stay_put(float X, float Y, float Z);
void movement_pause(void);
unsigned char movement_is_paused(void);
void movement_unpause(void);

#endif /* MOVEMENT_H_ */
